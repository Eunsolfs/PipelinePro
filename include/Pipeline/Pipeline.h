#pragma once

#include "Pipeline/Common.h"
#include "Pipeline/Node.h"
#include "Pipeline/Task.h"
#include "Pipeline/VariableManager.h"

namespace Pipeline {

// 前向声明
namespace nlohmann {
    template<typename T>
    class basic_json;

    using json = basic_json<>;
}

// 流水线状态枚举
enum class PipelineState {
    Stopped,    // 停止状态
    Running,    // 运行状态
    Suspended   // 暂停状态
};

// Pipeline类，管理节点的执行
class PIPELINE_API Pipeline {
public:
    Pipeline();
    ~Pipeline();

    // 获取当前流水线实例
    static Pipeline* getInstance() { return s_instance; }

    // 从JSON文件加载流水线
    bool loadFromFile(const std::string& filePath);

    // 从JSON字符串加载流水线
    bool loadFromString(const std::string& jsonString);

    // 通过名称获取节点
    std::shared_ptr<Node> getNode(const std::string& name) const;

    // 从特定节点开始执行流水线
    Task execute(const std::string& startNodeName);

    // 停止当前执行
    void stop();

    // 暂停当前执行
    void suspend();

    // 继续执行
    void resume();

    // 获取当前状态
    PipelineState getState() const { return m_state; }

    // 获取当前节点名称
    std::string getCurrentNodeName() const { return m_currentNodeName; }

    // 获取变量管理器
    VariableManager& getVariableManager() { return m_variableManager; }

    // 设置任务停止事件的回调
    using TaskStopCallback = std::function<void(const std::string& nodeName, const std::string& reason)>;
    void setTaskStopCallback(TaskStopCallback callback) { m_taskStopCallback = callback; }

    // 触发任务停止事件
    void triggerTaskStop(const std::string& nodeName, const std::string& reason = "");

    // 初始化全局变量
    bool initializeGlobalVariables(const nlohmann::json& json);

private:
    static Pipeline* s_instance;                    // 当前实例

    std::map<std::string, std::shared_ptr<Node>> m_nodes;
    VariableManager m_variableManager; // 变量管理器
    PipelineState m_state = PipelineState::Stopped; // 当前状态
    std::string m_currentNodeName;                  // 当前节点名称
    std::shared_ptr<Node> m_currentNode;            // 当前节点
    TaskAwaiter m_awaiter;                          // 协程等待器
    std::vector<std::string> m_globalVariables;     // 全局变量定义
    TaskStopCallback m_taskStopCallback;            // 任务停止回调

    // 解析JSON的辅助方法
    bool parseJson(const nlohmann::json& json);

    // 初始化节点变量
    void initializeNodeVariables(const std::shared_ptr<Node>& node);

    // 设置当前节点
    void setCurrentNode(const std::string& nodeName);
    void setCurrentNode(const std::shared_ptr<Node>& node);
};

} // namespace Pipeline
