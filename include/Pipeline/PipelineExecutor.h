#pragma once

#include "Pipeline/Common.h"
#include "Pipeline/Pipeline.h"
#include "Pipeline/Task.h"

namespace Pipeline {

// PipelineExecutor类，管理流水线的执行
class PIPELINE_API PipelineExecutor {
public:
    PipelineExecutor();
    ~PipelineExecutor();

    // 从文件加载并执行流水线
    bool executeFromFile(const std::string& filePath, const std::string& startNodeName);

    // 从字符串加载并执行流水线
    bool executeFromString(const std::string& jsonString, const std::string& startNodeName);

    // 停止当前执行
    void stop();

    // 暂停当前执行
    void suspend();

    // 继续执行
    void resume();

    // 获取当前状态
    PipelineState getState() const;

    // 获取当前节点名称
    std::string getCurrentNodeName() const;

    // 设置节点执行事件的回调
    using NodeCallback = std::function<void(const std::string& nodeName, bool success)>;
    void setNodeCallback(NodeCallback callback);

    // 设置任务停止事件的回调
    using TaskStopCallback = std::function<void(const std::string& nodeName, const std::string& reason)>;
    void setTaskStopCallback(TaskStopCallback callback);

private:
    std::unique_ptr<Pipeline> m_pipeline;
    NodeCallback m_nodeCallback;
    TaskStopCallback m_taskStopCallback;
    Task m_currentTask;
};

} // namespace Pipeline
