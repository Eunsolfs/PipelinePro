#include "Pipeline/Node.h"
#include "Pipeline/VariableManager.h"
#include <nlohmann/json.hpp>
#include <thread>
#include <iostream>

namespace Pipeline {

Node::Node(const std::string& name) : m_name(name) {
}

bool Node::initialize(const nlohmann::json& config, std::map<std::string, std::shared_ptr<Node>>& allNodes) {
    // 解析识别算法
    RecognitionType recognitionType = RecognitionType::DirectHit;
    nlohmann::json recognitionConfig;

    if (config.contains("recognition")) {
        if (config["recognition"].is_string()) {
            // 兼容旧格式："recognition": "DirectHit"
            recognitionType = stringToRecognitionType(config["recognition"].get<std::string>());
            recognitionConfig = config; // 使用整个节点配置作为识别算法参数
        } else if (config["recognition"].is_object()) {
            // 新格式："recognition": {"type": "DirectHit", ...}
            const auto& recognitionObj = config["recognition"];
            if (recognitionObj.contains("type")) {
                recognitionType = stringToRecognitionType(recognitionObj["type"].get<std::string>());
                recognitionConfig = recognitionObj; // 使用recognition对象作为识别算法参数
            }
        }
    }

    // 创建识别算法对象
    m_recognition = Recognition::create(recognitionType, {});

    // 将识别算法的参数传递给Recognition类进行解析
    if (m_recognition) {
        m_recognition->parseConfig(recognitionConfig);
    }

    // 解析动作
    ActionType actionType = ActionType::DoNothing;
    nlohmann::json actionConfig;

    if (config.contains("action")) {
        if (config["action"].is_string()) {
            // 兼容旧格式："action": "Click"
            actionType = stringToActionType(config["action"].get<std::string>());
            actionConfig = config; // 使用整个节点配置作为动作参数
        } else if (config["action"].is_object()) {
            // 新格式："action": {"type": "Click", ...}
            const auto& actionObj = config["action"];
            if (actionObj.contains("type")) {
                actionType = stringToActionType(actionObj["type"].get<std::string>());
                actionConfig = actionObj; // 使用action对象作为动作参数
            }
        }
    }

    // 创建动作对象
    m_action = Action::create(actionType, {});

    // 将动作的参数传递给Action类进行解析
    if (m_action) {
        m_action->setVariableManager(&m_variableManager);
        m_action->parseConfig(actionConfig);
    }

    // 解析后继节点
    if (config.contains("next")) {
        if (config["next"].is_string()) {
            m_nextNodes.push_back(config["next"].get<std::string>());
        } else if (config["next"].is_array()) {
            m_nextNodes = config["next"].get<std::vector<std::string>>();
        }
    }

    // 解析中断节点
    if (config.contains("interrupt")) {
        if (config["interrupt"].is_string()) {
            m_interruptNodes.push_back(config["interrupt"].get<std::string>());
        } else if (config["interrupt"].is_array()) {
            m_interruptNodes = config["interrupt"].get<std::vector<std::string>>();
        }
    }

    // 解析错误处理节点
    if (config.contains("on_error")) {
        if (config["on_error"].is_string()) {
            m_onErrorNodes.push_back(config["on_error"].get<std::string>());
        } else if (config["on_error"].is_array()) {
            m_onErrorNodes = config["on_error"].get<std::vector<std::string>>();
        }
    }

    // 解析变量定义
    if (config.contains("var")) {
        if (config["var"].is_string()) {
            m_variableDefinitions.push_back(config["var"].get<std::string>());
        } else if (config["var"].is_array()) {
            m_variableDefinitions = config["var"].get<std::vector<std::string>>();
        }
    }

    // 解析条件
    if (config.contains("condition")) {
        m_condition = config["condition"].get<std::string>();
    }

    // 解析条件处理
    if (config.contains("condition_process") && config["condition_process"].is_object()) {
        m_conditionProcess = config["condition_process"];

        // 解析true分支
        if (m_conditionProcess.contains("true") && m_conditionProcess["true"].is_object()) {
            const auto& trueObj = m_conditionProcess["true"];

            // 解析override_next
            if (trueObj.contains("override_next")) {
                if (trueObj["override_next"].is_string()) {
                    m_overrideNextNodes.push_back(trueObj["override_next"].get<std::string>());
                } else if (trueObj["override_next"].is_array()) {
                    m_overrideNextNodes = trueObj["override_next"].get<std::vector<std::string>>();
                }
            }

            // 解析override_interrupt
            if (trueObj.contains("override_interrupt")) {
                if (trueObj["override_interrupt"].is_string()) {
                    m_overrideInterruptNodes.push_back(trueObj["override_interrupt"].get<std::string>());
                } else if (trueObj["override_interrupt"].is_array()) {
                    m_overrideInterruptNodes = trueObj["override_interrupt"].get<std::vector<std::string>>();
                }
            }

            // 解析var_operation
            if (trueObj.contains("var_operation")) {
                m_varOperations["true"] = trueObj["var_operation"].get<std::string>();
            }

            // 解析condition_log
            if (trueObj.contains("condition_log")) {
                m_conditionLogs["true"] = trueObj["condition_log"].get<std::string>();
            }
        }

        // 解析false分支
        if (m_conditionProcess.contains("false") && m_conditionProcess["false"].is_object()) {
            const auto& falseObj = m_conditionProcess["false"];

            // 解析override_next
            if (falseObj.contains("override_next")) {
                if (falseObj["override_next"].is_string()) {
                    // 不直接设置，因为只有在条件为false时才会使用
                } else if (falseObj["override_next"].is_array()) {
                    // 不直接设置，因为只有在条件为false时才会使用
                }
            }

            // 解析override_interrupt
            if (falseObj.contains("override_interrupt")) {
                if (falseObj["override_interrupt"].is_string()) {
                    // 不直接设置，因为只有在条件为false时才会使用
                } else if (falseObj["override_interrupt"].is_array()) {
                    // 不直接设置，因为只有在条件为false时才会使用
                }
            }

            // 解析var_operation
            if (falseObj.contains("var_operation")) {
                m_varOperations["false"] = falseObj["var_operation"].get<std::string>();
            }

            // 解析condition_log
            if (falseObj.contains("condition_log")) {
                m_conditionLogs["false"] = falseObj["condition_log"].get<std::string>();
            }
        }
    }

    // 解析日志
    if (config.contains("log") && config["log"].is_object()) {
        for (auto it = config["log"].begin(); it != config["log"].end(); ++it) {
            m_logs[it.key()] = it.value().get<std::string>();
        }
    }

    // 解析是否启用
    if (config.contains("enabled")) {
        m_enabled = config["enabled"].get<bool>();
    }

    // 解析是否反转
    if (config.contains("inverse")) {
        m_inverse = config["inverse"].get<bool>();
        if (m_recognition) {
            m_recognition->setInverse(m_inverse);
        }
    }

    // 解析超时
    if (config.contains("timeout")) {
        m_timeout = config["timeout"].get<uint32_t>();
    }

    // 解析前置延迟
    if (config.contains("pre_delay")) {
        m_preDelay = config["pre_delay"].get<uint32_t>();
    }

    // 解析后置延迟
    if (config.contains("post_delay")) {
        m_postDelay = config["post_delay"].get<uint32_t>();
    }

    // 解析是否关注
    if (config.contains("focus")) {
        m_focus = config["focus"].get<bool>();
    }

    return true;
}

RecognitionResult Node::executeRecognition() {
    if (!m_enabled || !m_recognition) {
        RecognitionResult result;
        result.success = false;
        return result;
    }

    // 执行前置延迟
    if (m_preDelay > 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(m_preDelay));
    }

    // 执行识别
    return m_recognition->recognize();
}

std::vector<RecognitionResult> Node::executeRecognitionBatch() {
    std::vector<RecognitionResult> results;

    if (!m_enabled || !m_recognition) {
        return results;
    }

    // 执行前置延迟
    if (m_preDelay > 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(m_preDelay));
    }

    // 检查是否是OCR识别
    auto ocrRecognition = dynamic_cast<OCRRecognition*>(m_recognition.get());
    if (ocrRecognition) {
        // 执行批量OCR识别
        return ocrRecognition->recognizeBatch();
    } else {
        // 如果不是OCR识别，则执行普通识别
        auto result = m_recognition->recognize();
        if (result.success) {
            results.push_back(result);
        }
    }

    return results;
}

bool Node::executeAction(const RecognitionResult& result) {
    if (!m_enabled || !m_action) {
        return false;
    }

    // 执行动作
    bool success = m_action->execute(result);

    // 执行后置延迟
    if (m_postDelay > 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(m_postDelay));
    }

    return success;
}

// 检查条件是否满足
bool Node::checkCondition(VariableManager& variableManager) const {
    // 如果没有条件，直接返回true
    if (m_condition.empty()) {
        return true;
    }

    // 计算条件表达式
    return variableManager.evaluateCondition(m_condition);
}

// 处理条件过程
void Node::processCondition(VariableManager& variableManager, bool conditionResult) {
    // 如果没有条件处理配置，直接返回
    if (m_conditionProcess.empty()) {
        return;
    }

    // 清除之前的重写节点
    m_overrideNextNodes.clear();
    m_overrideInterruptNodes.clear();

    // 根据条件结果选择分支
    std::string branch = conditionResult ? "true" : "false";

    // 如果对应分支存在
    if (m_conditionProcess.contains(branch) && m_conditionProcess[branch].is_object()) {
        const auto& branchObj = m_conditionProcess[branch];

        // 处理override_next
        if (branchObj.contains("override_next")) {
            if (branchObj["override_next"].is_string()) {
                m_overrideNextNodes.push_back(branchObj["override_next"].get<std::string>());
            } else if (branchObj["override_next"].is_array()) {
                m_overrideNextNodes = branchObj["override_next"].get<std::vector<std::string>>();
            }
        }

        // 处理override_interrupt
        if (branchObj.contains("override_interrupt")) {
            if (branchObj["override_interrupt"].is_string()) {
                m_overrideInterruptNodes.push_back(branchObj["override_interrupt"].get<std::string>());
            } else if (branchObj["override_interrupt"].is_array()) {
                m_overrideInterruptNodes = branchObj["override_interrupt"].get<std::vector<std::string>>();
            }
        }
    }

    // 执行变量操作
    executeVarOperation(variableManager, conditionResult);

    // 处理条件日志
    processConditionLog(variableManager, conditionResult);
}

// 处理条件日志
void Node::processConditionLog(VariableManager& variableManager, bool conditionResult) const {
    // 如果没有条件日志配置，直接返回
    if (m_conditionLogs.empty()) {
        return;
    }

    // 根据条件结果选择日志
    std::string logKey = conditionResult ? "true" : "false";
    auto it = m_conditionLogs.find(logKey);

    if (it != m_conditionLogs.end()) {
        // 处理日志字符串，替换变量并执行操作
        std::string processedLog = variableManager.processLogString(it->second);

        // 输出日志
        std::cout << "[" << m_name << "] Condition " << logKey << ": " << processedLog << std::endl;
    }
}

// 执行变量操作
void Node::executeVarOperation(VariableManager& variableManager, bool conditionResult) const {
    // 如果没有变量操作配置，直接返回
    if (m_varOperations.empty()) {
        return;
    }

    // 根据条件结果选择变量操作
    std::string opKey = conditionResult ? "true" : "false";
    auto it = m_varOperations.find(opKey);

    if (it != m_varOperations.end()) {
        // 执行变量操作
        variableManager.processLogString(it->second);
    }
}

// 处理日志
void Node::processLog(VariableManager& variableManager, bool success) const {
    // 如果没有日志配置，直接返回
    if (m_logs.empty()) {
        return;
    }

    // 根据执行结果选择日志
    std::string logKey = success ? "true" : "false";
    auto it = m_logs.find(logKey);

    if (it != m_logs.end()) {
        // 处理日志字符串，替换变量并执行操作
        std::string processedLog = variableManager.processLogString(it->second);

        // 输出日志
        std::cout << "[" << m_name << "] " << processedLog << std::endl;
    }
}

} // namespace Pipeline
