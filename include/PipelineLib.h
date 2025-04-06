#pragma once

// 包含所有必要的头文件
#include "Pipeline/Common.h"
#include "Pipeline/Recognition.h"
#include "Pipeline/Action.h"
#include "Pipeline/Node.h"
#include "Pipeline/Task.h"
#include "Pipeline/Pipeline.h"
#include "Pipeline/PipelineExecutor.h"

// 导出函数
extern "C" {
    // 初始化库
    PIPELINE_API bool PipelineInit();

    // 清理库
    PIPELINE_API void PipelineCleanup();

    // 创建流水线执行器
    PIPELINE_API Pipeline::PipelineExecutor* PipelineCreateExecutor();

    // 销毁流水线执行器
    PIPELINE_API void PipelineDestroyExecutor(Pipeline::PipelineExecutor* executor);

    // 从文件执行流水线
    PIPELINE_API bool PipelineExecuteFromFile(Pipeline::PipelineExecutor* executor, const char* filePath, const char* startNodeName);

    // 从字符串执行流水线
    PIPELINE_API bool PipelineExecuteFromString(Pipeline::PipelineExecutor* executor, const char* jsonString, const char* startNodeName);

    // 停止流水线执行
    PIPELINE_API void PipelineStop(Pipeline::PipelineExecutor* executor);

    // 暂停流水线执行
    PIPELINE_API void PipelineSuspend(Pipeline::PipelineExecutor* executor);

    // 继续流水线执行
    PIPELINE_API void PipelineContinue(Pipeline::PipelineExecutor* executor);

    // 获取流水线状态
    PIPELINE_API int PipelineGetState(Pipeline::PipelineExecutor* executor);

    // 获取当前节点名称
    PIPELINE_API const char* PipelineGetCurrentNodeName(Pipeline::PipelineExecutor* executor);

    // 设置任务停止回调
    typedef void (*PipelineTaskStopCallbackFunc)(const char* nodeName, const char* reason);
    PIPELINE_API void PipelineSetTaskStopCallback(Pipeline::PipelineExecutor* executor, PipelineTaskStopCallbackFunc callback);
}
