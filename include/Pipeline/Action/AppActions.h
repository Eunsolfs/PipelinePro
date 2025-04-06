#pragma once

#include "Pipeline/Action/Action.h"

namespace Pipeline {

// StartApp动作类 - 启动应用
class PIPELINE_API StartAppAction : public Action {
public:
    StartAppAction();
    virtual bool execute(const RecognitionResult& result) override;
    virtual bool parseConfig(const nlohmann::json& config) override;

private:
    std::string m_package;
};

// StopApp动作类 - 停止应用
class PIPELINE_API StopAppAction : public Action {
public:
    StopAppAction();
    virtual bool execute(const RecognitionResult& result) override;
    virtual bool parseConfig(const nlohmann::json& config) override;

private:
    std::string m_package;
};

} // namespace Pipeline
