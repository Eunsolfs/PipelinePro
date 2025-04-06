#pragma once

#include "Pipeline/Action/Action.h"

namespace Pipeline {

// DoNothing动作类 - 什么都不做
class PIPELINE_API DoNothingAction : public Action {
public:
    DoNothingAction();
    virtual bool execute(const RecognitionResult& result) override;
    virtual bool parseConfig(const nlohmann::json& config) override;
};

// StopTask动作类 - 停止任务
class PIPELINE_API StopTaskAction : public Action {
public:
    StopTaskAction();
    virtual bool execute(const RecognitionResult& result) override;
    virtual bool parseConfig(const nlohmann::json& config) override;
};

} // namespace Pipeline
