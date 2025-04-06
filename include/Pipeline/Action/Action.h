#pragma once

#include "Pipeline/Common.h"
#include <memory>
#include <variant>
#include <string>
#include <vector>

namespace Pipeline {

// 前向声明
class RecognitionResult;

// 动作类型枚举
enum class ActionType {
    DoNothing,   // 什么都不做
    Click,       // 点击
    Swipe,       // 滑动
    Key,         // 按键
    Text,        // 输入文本
    StartApp,    // 启动应用
    StopApp,     // 停止应用
    StopTask,    // 停止任务
    Command      // 执行命令
};

// 动作基类
class PIPELINE_API Action {
public:
    // 构造函数
    Action(ActionType type);
    virtual ~Action() = default;

    // 获取动作类型
    ActionType getType() const { return m_type; }

    // 纯虚函数，由派生类实现
    virtual bool execute(const RecognitionResult& result) = 0;
    
    // 解析参数，由派生类实现
    virtual bool parseConfig(const nlohmann::json& config) = 0;
    
    // 设置变量管理器
    void setVariableManager(VariableManager* variableManager) { m_variableManager = variableManager; }
    
    // 工厂方法，根据类型创建动作对象
    static std::unique_ptr<Action> create(ActionType type, const nlohmann::json& config);

protected:
    ActionType m_type;
    VariableManager* m_variableManager = nullptr;
};

// 将字符串转换为动作类型
ActionType stringToActionType(const std::string& typeStr);

// 将动作类型转换为字符串
std::string actionTypeToString(ActionType type);

} // namespace Pipeline
