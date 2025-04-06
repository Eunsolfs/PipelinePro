#include "Pipeline/Action/BasicActions.h"
#include "Pipeline/RecognitionResult.h"
#include "Pipeline/PipelineExecutor.h"

namespace Pipeline {

// DoNothingAction实现
DoNothingAction::DoNothingAction() : Action(ActionType::DoNothing) {
}

bool DoNothingAction::execute(const RecognitionResult& result) {
    // 什么都不做，直接返回成功
    return true;
}

bool DoNothingAction::parseConfig(const nlohmann::json& config) {
    // DoNothingAction不需要额外的参数
    return true;
}

// StopTaskAction实现
StopTaskAction::StopTaskAction() : Action(ActionType::StopTask) {
}

bool StopTaskAction::parseConfig(const nlohmann::json& config) {
    // StopTaskAction不需要额外的参数
    return true;
}

bool StopTaskAction::execute(const RecognitionResult& result) {
    // 停止当前任务
    // 在实际实现中，这里应该调用PipelineExecutor的stop方法
    // 由于我们无法直接访问PipelineExecutor实例，这里通过全局函数实现
    PipelineStop(nullptr);
    return true;
}

} // namespace Pipeline
