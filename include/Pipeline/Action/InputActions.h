#pragma once

#include "Pipeline/Action/Action.h"

namespace Pipeline {

// Click动作类 - 点击
class PIPELINE_API ClickAction : public Action {
public:
    ClickAction();
    virtual bool execute(const RecognitionResult& result) override;
    virtual bool parseConfig(const nlohmann::json& config) override;

private:
    std::variant<bool, std::string, std::vector<int>> m_target = true;
    std::vector<int> m_targetOffset = {0, 0, 0, 0};
};

// Swipe动作类 - 滑动
class PIPELINE_API SwipeAction : public Action {
public:
    SwipeAction();
    virtual bool execute(const RecognitionResult& result) override;
    virtual bool parseConfig(const nlohmann::json& config) override;

private:
    std::variant<bool, std::string, std::vector<int>> m_begin = true;
    std::vector<int> m_beginOffset = {0, 0, 0, 0};
    std::variant<bool, std::string, std::vector<int>> m_end;
    std::vector<int> m_endOffset = {0, 0, 0, 0};
    uint32_t m_duration = 200;
};

// Key动作类 - 按键
class PIPELINE_API KeyAction : public Action {
public:
    KeyAction();
    virtual bool execute(const RecognitionResult& result) override;
    virtual bool parseConfig(const nlohmann::json& config) override;

private:
    std::vector<int> m_keys;
};

// Text动作类 - 输入文本
class PIPELINE_API TextAction : public Action {
public:
    TextAction();
    virtual bool execute(const RecognitionResult& result) override;
    virtual bool parseConfig(const nlohmann::json& config) override;

private:
    std::string m_inputText;
};

} // namespace Pipeline
