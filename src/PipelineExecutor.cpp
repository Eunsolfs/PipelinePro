#include "Pipeline/PipelineExecutor.h"

namespace Pipeline {

PipelineExecutor::PipelineExecutor(){
    m_pipeline = std::make_unique<Pipeline>();
    // 初始化时设置流水线的任务停止回调
    m_pipeline->setTaskStopCallback([this](const std::string& nodeName, const std::string& reason) {
        if (m_taskStopCallback) {
            m_taskStopCallback(nodeName, reason);
        }
    });
}

PipelineExecutor::~PipelineExecutor() {
    stop();
}

bool PipelineExecutor::executeFromFile(const std::string& filePath, const std::string& startNodeName) {
    // 停止当前执行
    stop();

    // 加载流水线
    if (!m_pipeline->loadFromFile(filePath)) {
        return false;
    }

    // 执行流水线
    m_currentTask = m_pipeline->execute(startNodeName);

    return true;
}

bool PipelineExecutor::executeFromString(const std::string& jsonString, const std::string& startNodeName) {
    // 停止当前执行
    stop();

    // 加载流水线
    if (!m_pipeline->loadFromString(jsonString)) {
        return false;
    }

    // 执行流水线
    m_currentTask = m_pipeline->execute(startNodeName);

    return true;
}

void PipelineExecutor::stop() {
    if (m_pipeline) {
        m_pipeline->stop();
    }
}

void PipelineExecutor::suspend() {
    if (m_pipeline) {
        m_pipeline->suspend();
    }
}

void PipelineExecutor::resume() {
    if (m_pipeline) {
        m_pipeline->resume();
    }
}

PipelineState PipelineExecutor::getState() const {
    if (m_pipeline) {
        return m_pipeline->getState();
    }
    return PipelineState::Stopped;
}

std::string PipelineExecutor::getCurrentNodeName() const {
    if (m_pipeline) {
        return m_pipeline->getCurrentNodeName();
    }
    return "";
}

void PipelineExecutor::setNodeCallback(NodeCallback callback) {
    m_nodeCallback = std::move(callback);
}

void PipelineExecutor::setTaskStopCallback(TaskStopCallback callback) {
    m_taskStopCallback = std::move(callback);

    // 设置流水线的任务停止回调
    if (m_pipeline) {
        m_pipeline->setTaskStopCallback([this](const std::string& nodeName, const std::string& reason) {
            if (m_taskStopCallback) {
                m_taskStopCallback(nodeName, reason);
            }
        });
    }
}

} // namespace Pipeline
