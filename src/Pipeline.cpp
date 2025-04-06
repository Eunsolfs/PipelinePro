#include "Pipeline/Pipeline.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <sstream>
#include <chrono>
#include <thread>

namespace Pipeline {

// 初始化静态实例
Pipeline* Pipeline::s_instance = nullptr;

Pipeline::Pipeline() : m_state(PipelineState::Stopped) {
    // 设置当前实例
    s_instance = this;
}

Pipeline::~Pipeline() {
    stop();

    // 清除当前实例
    if (s_instance == this) {
        s_instance = nullptr;
    }
}

bool Pipeline::loadFromFile(const std::string& filePath) {
    try {
        // 打开文件
        std::ifstream file(filePath);
        if (!file.is_open()) {
            return false;
        }

        // 读取文件内容
        std::stringstream buffer;
        buffer << file.rdbuf();
        file.close();

        // 解析JSON
        nlohmann::json json = nlohmann::json::parse(buffer.str());
        return parseJson(json);
    } catch (const std::exception& e) {
        // 处理异常
        return false;
    }
}

bool Pipeline::loadFromString(const std::string& jsonString) {
    try {
        // 解析JSON
        nlohmann::json json = nlohmann::json::parse(jsonString);
        return parseJson(json);
    } catch (const std::exception& e) {
        // 处理异常
        return false;
    }
}

std::shared_ptr<Node> Pipeline::getNode(const std::string& name) const {
    auto it = m_nodes.find(name);
    if (it != m_nodes.end()) {
        return it->second;
    }
    return nullptr;
}

Task Pipeline::execute(const std::string& startNodeName) {
    // 使用协程实现流水线执行
    auto executeTask = [this](const std::string& nodeName) -> Task {
        // 设置状态为运行中
        m_state = PipelineState::Running;

        // 设置当前节点
        setCurrentNode(nodeName);

        // 检查节点是否存在
        if (!m_currentNode) {
            m_state = PipelineState::Stopped;
            co_return;
        }

        // 初始化节点变量
        initializeNodeVariables(m_currentNode);

        // 执行流水线
        while (m_state == PipelineState::Running && m_currentNode) {
            // 检查节点是否启用
            if (!m_currentNode->isEnabled()) {
                m_state = PipelineState::Stopped;
                co_return;
            }

            // 检查条件是否满足
            bool conditionResult = m_currentNode->checkCondition(m_variableManager);

            // 处理条件过程
            m_currentNode->processCondition(m_variableManager, conditionResult);

            if (!conditionResult) {
                // 如果条件不满足，尝试执行中断节点
                const auto& interruptNodes = m_currentNode->getInterruptNodes(); // 这里已经是可能被重写后的节点列表
                if (!interruptNodes.empty()) {
                    setCurrentNode(interruptNodes[0]);
                    continue;
                } else {
                    // 如果没有中断节点，跳过当前节点
                    const auto& nextNodes = m_currentNode->getNextNodes(); // 这里已经是可能被重写后的节点列表
                    if (!nextNodes.empty()) {
                        setCurrentNode(nextNodes[0]);
                        continue;
                    } else {
                        m_state = PipelineState::Stopped;
                        co_return;
                    }
                }
            }

            // 执行节点的识别
            auto result = m_currentNode->executeRecognition();

            // 如果识别成功，执行动作
            if (result) {
                bool actionSuccess = m_currentNode->executeAction(result);

                // 处理日志
                m_currentNode->processLog(m_variableManager, actionSuccess);

                if (!actionSuccess) {
                    // 如果动作执行失败，尝试执行错误处理节点
                    const auto& onErrorNodes = m_currentNode->getOnErrorNodes();
                    if (!onErrorNodes.empty()) {
                        setCurrentNode(onErrorNodes[0]);
                        continue;
                    } else {
                        m_state = PipelineState::Stopped;
                        co_return;
                    }
                }

                // 获取后继节点
                const auto& nextNodes = m_currentNode->getNextNodes();
                if (nextNodes.empty()) {
                    // 如果没有后继节点，任务完成
                    m_state = PipelineState::Stopped;
                    co_return;
                }

                // 尝试识别后继节点
                bool foundNext = false;
                for (const auto& nextNodeName : nextNodes) {
                    auto nextNode = getNode(nextNodeName);
                    if (nextNode && nextNode->isEnabled()) {
                        auto nextResult = nextNode->executeRecognition();
                        if (nextResult) {
                            // 找到匹配的后继节点，切换到该节点
                            setCurrentNode(nextNode);
                            foundNext = true;
                            break;
                        }
                    }
                }

                // 如果没有找到匹配的后继节点，尝试中断节点
                if (!foundNext) {
                    const auto& interruptNodes = m_currentNode->getInterruptNodes();
                    if (!interruptNodes.empty()) {
                        for (const auto& interruptNodeName : interruptNodes) {
                            auto interruptNode = getNode(interruptNodeName);
                            if (interruptNode && interruptNode->isEnabled()) {
                                auto interruptResult = interruptNode->executeRecognition();
                                if (interruptResult) {
                                    // 找到匹配的中断节点，切换到该节点
                                    setCurrentNode(interruptNode);
                                    foundNext = true;
                                    break;
                                }
                            }
                        }
                    }
                }

                // 如果仍然没有找到匹配的节点，等待一段时间后重试
                if (!foundNext) {
                    // 检查是否超时
                    auto startTime = std::chrono::steady_clock::now();
                    while (!foundNext && m_state == PipelineState::Running) {
                        // 等待一段时间
                        std::this_thread::sleep_for(std::chrono::milliseconds(100));

                        // 如果状态变为暂停，则暂停执行
                        if (m_state == PipelineState::Suspended) {
                            // 保存当前等待器，以便稍后恢复
                            m_awaiter = co_await TaskAwaiter{};

                            // 检查是否已经停止
                            if (m_state == PipelineState::Stopped) {
                                co_return;
                            }
                        }

                        // 重新尝试识别后继节点
                        for (const auto& nextNodeName : nextNodes) {
                            auto nextNode = getNode(nextNodeName);
                            if (nextNode && nextNode->isEnabled()) {
                                auto nextResult = nextNode->executeRecognition();
                                if (nextResult) {
                                    // 找到匹配的后继节点，切换到该节点
                                    setCurrentNode(nextNode);
                                    foundNext = true;
                                    break;
                                }
                            }
                        }

                        // 如果仍然没有找到匹配的后继节点，尝试中断节点
                        if (!foundNext) {
                            const auto& interruptNodes = m_currentNode->getInterruptNodes();
                            if (!interruptNodes.empty()) {
                                for (const auto& interruptNodeName : interruptNodes) {
                                    auto interruptNode = getNode(interruptNodeName);
                                    if (interruptNode && interruptNode->isEnabled()) {
                                        auto interruptResult = interruptNode->executeRecognition();
                                        if (interruptResult) {
                                            // 找到匹配的中断节点，切换到该节点
                                            setCurrentNode(interruptNode);
                                            foundNext = true;
                                            break;
                                        }
                                    }
                                }
                            }
                        }

                        // 检查是否超时
                        auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(
                            std::chrono::steady_clock::now() - startTime).count();
                        if (elapsedTime > m_currentNode->getTimeout()) {
                            // 超时，尝试执行错误处理节点
                            const auto& onErrorNodes = m_currentNode->getOnErrorNodes();
                            if (!onErrorNodes.empty()) {
                                setCurrentNode(onErrorNodes[0]);
                                foundNext = true;
                                break;
                            } else {
                                // 如果没有错误处理节点，任务失败
                                m_state = PipelineState::Stopped;
                                co_return;
                            }
                        }
                    }
                }

                // 如果没有找到下一个节点，任务完成
                if (!foundNext) {
                    m_state = PipelineState::Stopped;
                    co_return;
                }
            } else {
                // 如果识别失败，尝试执行错误处理节点
                const auto& onErrorNodes = m_currentNode->getOnErrorNodes();
                if (!onErrorNodes.empty()) {
                    setCurrentNode(onErrorNodes[0]);
                } else {
                    // 如果没有错误处理节点，任务失败
                    m_state = PipelineState::Stopped;
                    co_return;
                }
            }

            // 让出执行权，允许其他协程执行
            // 如果状态为暂停，则暂停执行
            if (m_state == PipelineState::Suspended) {
                // 保存当前等待器，以便稍后恢复
                m_awaiter = co_await TaskAwaiter{};

                // 检查是否已经停止
                if (m_state == PipelineState::Stopped) {
                    co_return;
                }
            } else {
                co_await std::suspend_always{};
            }
        }

        m_state = PipelineState::Stopped;
    };

    return executeTask(startNodeName);
}

// 设置当前节点
void Pipeline::setCurrentNode(const std::string& nodeName) {
    m_currentNodeName = nodeName;
    m_currentNode = getNode(nodeName);
}

void Pipeline::setCurrentNode(const std::shared_ptr<Node>& node) {
    if (node) {
        m_currentNode = node;
        m_currentNodeName = node->getName();
    } else {
        m_currentNode = nullptr;
        m_currentNodeName = "";
    }
}

// 停止流水线执行
void Pipeline::stop() {
    m_state = PipelineState::Stopped;
    m_currentNodeName = "";
    m_currentNode = nullptr;

    // 恢复协程，如果它处于暂停状态
    if (m_awaiter.m_handle) {
        m_awaiter.resume();
    }
}

// 暂停流水线执行
void Pipeline::suspend() {
    if (m_state == PipelineState::Running) {
        m_state = PipelineState::Suspended;
    }
}

// 继续流水线执行
void Pipeline::resume() {
    if (m_state == PipelineState::Suspended && m_awaiter.m_handle) {
        m_state = PipelineState::Running;
        m_awaiter.resume();
    }
}

// 触发任务停止事件
void Pipeline::triggerTaskStop(const std::string& nodeName, const std::string& reason) {
    // 如果设置了回调，则触发回调
    if (m_taskStopCallback) {
        m_taskStopCallback(nodeName, reason);
    }
}

// 初始化节点变量
void Pipeline::initializeNodeVariables(const std::shared_ptr<Node>& node) {
    // 获取节点的变量定义
    const auto& varDefs = node->getVariableDefinitions();

    // 解析并初始化变量
    if (!varDefs.empty()) {
        m_variableManager.parseVariableList(varDefs);
    }
}

// 初始化全局变量
bool Pipeline::initializeGlobalVariables(const nlohmann::json& json) {
    // 清空全局变量定义
    m_globalVariables.clear();

    // 检查是否有全局变量定义
    if (json.contains("var_global")) {
        const auto& varGlobal = json["var_global"];

        // 解析全局变量定义
        if (varGlobal.is_string()) {
            m_globalVariables.push_back(varGlobal.get<std::string>());
        } else if (varGlobal.is_array()) {
            m_globalVariables = varGlobal.get<std::vector<std::string>>();
        }

        // 初始化全局变量
        if (!m_globalVariables.empty()) {
            return m_variableManager.parseVariableList(m_globalVariables);
        }
    }

    return true;
}

bool Pipeline::parseJson(const nlohmann::json& json) {
    try {
        // 清空现有节点
        m_nodes.clear();

        // 初始化全局变量
        if (!initializeGlobalVariables(json)) {
            return false;
        }

        // 创建所有节点
        for (auto it = json.begin(); it != json.end(); ++it) {
            const std::string& nodeName = it.key();
            // 跳过var_global字段
            if (nodeName == "var_global") {
                continue;
            }
            m_nodes[nodeName] = std::make_shared<Node>(nodeName);
        }

        // 初始化所有节点
        for (auto it = json.begin(); it != json.end(); ++it) {
            const std::string& nodeName = it.key();
            const nlohmann::json& nodeConfig = it.value();

            auto node = m_nodes[nodeName];
            if (!node->initialize(nodeConfig, m_nodes)) {
                return false;
            }

            // 初始化节点变量
            initializeNodeVariables(node);
        }

        return true;
    } catch (const std::exception& e) {
        // 处理异常
        return false;
    }
}

} // namespace Pipeline
