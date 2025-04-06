#pragma once

#include "Pipeline/Common.h"
#include "Pipeline/Recognition.h"
#include "Pipeline/Action.h"
#include "Pipeline/VariableManager.h"
#include <map>
#include <unordered_map>

namespace Pipeline {

// 前向声明
namespace nlohmann {
    template<typename T>
    class basic_json;

    using json = basic_json<>;
}

// 前向声明
class VariableManager;

// 节点类，表示流水线中的单个节点
class PIPELINE_API Node {
public:
    Node(const std::string& name);
    ~Node() = default;

    // 从JSON配置初始化节点
    bool initialize(const nlohmann::json& config, std::map<std::string, std::shared_ptr<Node>>& allNodes);

    // 执行节点的识别和动作
    RecognitionResult executeRecognition();
    std::vector<RecognitionResult> executeRecognitionBatch();
    bool executeAction(const RecognitionResult& result);

    // 检查条件是否满足
    bool checkCondition(VariableManager& variableManager) const;

    // 处理条件过程
    void processCondition(VariableManager& variableManager, bool conditionResult);

    // 处理条件日志
    void processConditionLog(VariableManager& variableManager, bool conditionResult) const;

    // 执行变量操作
    void executeVarOperation(VariableManager& variableManager, bool conditionResult) const;

    // 处理日志
    void processLog(VariableManager& variableManager, bool success) const;

    // 获取动态重写后的节点列表
    std::vector<std::string> getOverrideNextNodes() const { return m_overrideNextNodes; }
    std::vector<std::string> getOverrideInterruptNodes() const { return m_overrideInterruptNodes; }

    // 获取变量定义
    const std::vector<std::string>& getVariableDefinitions() const { return m_variableDefinitions; }

    // Getters
    const std::string& getName() const { return m_name; }
    const std::vector<std::string>& getNextNodes() const {
        // 如果有动态重写的next节点，则返回重写后的节点
        return m_overrideNextNodes.empty() ? m_nextNodes : m_overrideNextNodes;
    }
    const std::vector<std::string>& getInterruptNodes() const {
        // 如果有动态重写的interrupt节点，则返回重写后的节点
        return m_overrideInterruptNodes.empty() ? m_interruptNodes : m_overrideInterruptNodes;
    }
    const std::vector<std::string>& getOnErrorNodes() const { return m_onErrorNodes; }
    bool isEnabled() const { return m_enabled; }
    uint32_t getTimeout() const { return m_timeout; }
    uint32_t getPreDelay() const { return m_preDelay; }
    uint32_t getPostDelay() const { return m_postDelay; }
    bool isFocused() const { return m_focus; }

private:
    std::string m_name;
    std::unique_ptr<Recognition> m_recognition;
    std::unique_ptr<Action> m_action;
    std::vector<std::string> m_nextNodes;                // 原始的next节点列表
    std::vector<std::string> m_interruptNodes;          // 原始的interrupt节点列表
    std::vector<std::string> m_onErrorNodes;            // 错误处理节点列表
    std::vector<std::string> m_variableDefinitions;     // 变量定义列表
    std::string m_condition;                            // 条件表达式
    std::unordered_map<std::string, std::string> m_logs; // 日志配置

    // condition_process相关成员
    nlohmann::json m_conditionProcess;                  // 条件处理配置
    std::vector<std::string> m_overrideNextNodes;       // 动态重写的next节点列表
    std::vector<std::string> m_overrideInterruptNodes;   // 动态重写的interrupt节点列表
    std::unordered_map<std::string, std::string> m_conditionLogs; // 条件日志配置
    std::unordered_map<std::string, std::string> m_varOperations; // 变量操作配置
    bool m_enabled = true;
    bool m_inverse = false;
    uint32_t m_timeout = 20000; // 默认20秒
    uint32_t m_preDelay = 200;  // 默认200毫秒
    uint32_t m_postDelay = 200; // 默认200毫秒
    bool m_focus = false;
};

} // namespace Pipeline
