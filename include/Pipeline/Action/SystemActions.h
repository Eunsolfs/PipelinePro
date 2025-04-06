#pragma once

#include "Pipeline/Action/Action.h"

namespace Pipeline {

// Command动作类 - 执行命令
class PIPELINE_API CommandAction : public Action {
public:
    CommandAction();
    virtual bool execute(const RecognitionResult& result) override;
    virtual bool parseConfig(const nlohmann::json& config) override;

private:
    std::string m_exec;
    std::vector<std::string> m_args;
    bool m_detach = false;
};

} // namespace Pipeline
