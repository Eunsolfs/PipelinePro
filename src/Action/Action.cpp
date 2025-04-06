#include "Pipeline/Action/Action.h"
#include "Pipeline/Action/BasicActions.h"
#include "Pipeline/Action/InputActions.h"
#include "Pipeline/Action/AppActions.h"
#include "Pipeline/Action/SystemActions.h"
#include "Pipeline/RecognitionResult.h"

namespace Pipeline {

// 构造函数
Action::Action(ActionType type) : m_type(type) {
}

// 创建动作对象的工厂方法
std::unique_ptr<Action> Action::create(ActionType type, const nlohmann::json& config) {
    std::unique_ptr<Action> action;
    
    switch (type) {
        case ActionType::DoNothing:
            action = std::make_unique<DoNothingAction>();
            break;
        case ActionType::Click:
            action = std::make_unique<ClickAction>();
            break;
        case ActionType::Swipe:
            action = std::make_unique<SwipeAction>();
            break;
        case ActionType::Key:
            action = std::make_unique<KeyAction>();
            break;
        case ActionType::Text:
            action = std::make_unique<TextAction>();
            break;
        case ActionType::StartApp:
            action = std::make_unique<StartAppAction>();
            break;
        case ActionType::StopApp:
            action = std::make_unique<StopAppAction>();
            break;
        case ActionType::StopTask:
            action = std::make_unique<StopTaskAction>();
            break;
        case ActionType::Command:
            action = std::make_unique<CommandAction>();
            break;
        default:
            action = std::make_unique<DoNothingAction>();
            break;
    }
    
    // 解析配置
    if (action && !config.empty()) {
        action->parseConfig(config);
    }
    
    return action;
}

// 将字符串转换为动作类型
ActionType stringToActionType(const std::string& typeStr) {
    if (typeStr == "Click") {
        return ActionType::Click;
    } else if (typeStr == "Swipe") {
        return ActionType::Swipe;
    } else if (typeStr == "Key") {
        return ActionType::Key;
    } else if (typeStr == "Text") {
        return ActionType::Text;
    } else if (typeStr == "StartApp") {
        return ActionType::StartApp;
    } else if (typeStr == "StopApp") {
        return ActionType::StopApp;
    } else if (typeStr == "StopTask") {
        return ActionType::StopTask;
    } else if (typeStr == "Command") {
        return ActionType::Command;
    } else if (typeStr == "DoNothing") {
        return ActionType::DoNothing;
    } else {
        return ActionType::DoNothing; // 默认为DoNothing
    }
}

// 将动作类型转换为字符串
std::string actionTypeToString(ActionType type) {
    switch (type) {
        case ActionType::Click:
            return "Click";
        case ActionType::Swipe:
            return "Swipe";
        case ActionType::Key:
            return "Key";
        case ActionType::Text:
            return "Text";
        case ActionType::StartApp:
            return "StartApp";
        case ActionType::StopApp:
            return "StopApp";
        case ActionType::StopTask:
            return "StopTask";
        case ActionType::Command:
            return "Command";
        case ActionType::DoNothing:
            return "DoNothing";
        default:
            return "Unknown";
    }
}

} // namespace Pipeline
