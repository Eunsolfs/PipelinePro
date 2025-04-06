#include "PipelineLib.h"

// DLL导出函数实现
extern "C" {

// 初始化库
PIPELINE_API bool PipelineInit() {
    // 在这里进行库的初始化工作
    // 例如初始化日志系统、加载配置等
    return true;
}

// 清理库
PIPELINE_API void PipelineCleanup() {
    // 在这里进行库的清理工作
    // 例如释放资源、关闭日志等
}

// 创建流水线执行器
PIPELINE_API Pipeline::PipelineExecutor* PipelineCreateExecutor() {
    return new Pipeline::PipelineExecutor();
}

// 销毁流水线执行器
PIPELINE_API void PipelineDestroyExecutor(Pipeline::PipelineExecutor* executor) {
    if (executor) {
        delete executor;
    }
}

// 从文件执行流水线
PIPELINE_API bool PipelineExecuteFromFile(Pipeline::PipelineExecutor* executor, const char* filePath, const char* startNodeName) {
    if (!executor || !filePath || !startNodeName) {
        return false;
    }

    return executor->executeFromFile(filePath, startNodeName);
}

// 从字符串执行流水线
PIPELINE_API bool PipelineExecuteFromString(Pipeline::PipelineExecutor* executor, const char* jsonString, const char* startNodeName) {
    if (!executor || !jsonString || !startNodeName) {
        return false;
    }

    return executor->executeFromString(jsonString, startNodeName);
}

// 停止流水线执行
PIPELINE_API void PipelineStop(Pipeline::PipelineExecutor* executor) {
    if (executor) {
        executor->stop();
    }
}

// 暂停流水线执行
PIPELINE_API void PipelineSuspend(Pipeline::PipelineExecutor* executor) {
    if (executor) {
        executor->suspend();
    }
}

// 继续流水线执行
PIPELINE_API void PipelineContinue(Pipeline::PipelineExecutor* executor) {
    if (executor) {
        executor->resume();
    }
}

// 获取流水线状态
PIPELINE_API int PipelineGetState(Pipeline::PipelineExecutor* executor) {
    if (executor) {
        return static_cast<int>(executor->getState());
    }
    return static_cast<int>(Pipeline::PipelineState::Stopped);
}

// 获取当前节点名称
PIPELINE_API const char* PipelineGetCurrentNodeName(Pipeline::PipelineExecutor* executor) {
    static thread_local std::string nodeName;

    if (executor) {
        nodeName = executor->getCurrentNodeName();
        return nodeName.c_str();
    }

    nodeName.clear();
    return nodeName.c_str();
}

// 设置任务停止回调
PIPELINE_API void PipelineSetTaskStopCallback(Pipeline::PipelineExecutor* executor, PipelineTaskStopCallbackFunc callback) {
    if (executor && callback) {
        executor->setTaskStopCallback([callback](const std::string& nodeName, const std::string& reason) {
            callback(nodeName.c_str(), reason.c_str());
        });
    }
}

} // extern "C"
