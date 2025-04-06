#pragma once

#include "Pipeline/Common.h"
#include <unordered_map>
#include <variant>
#include <string>
#include <vector>
#include <optional>
#include <functional>
#include <regex>

namespace Pipeline {

// 变量类型枚举
enum class VariableType {
    Integer,    // 整数类型，以%i开头
    String,     // 字符串类型，以%s开头
    Float,      // 浮点数类型，以%f开头
    Boolean,    // 布尔类型，以%b开头
    Point,      // 点坐标类型，以%p开头，存储(x,y)坐标
    Rect        // 矩形区域类型，以%r开头，存储(x1,y1,x2,y2)坐标
};

// 点坐标结构体
struct PIPELINE_API Point {
    int x = 0;
    int y = 0;

    Point() = default;
    Point(int x, int y) : x(x), y(y) {}

    // 从字符串解析点坐标，格式为"x,y"
    static Point fromString(const std::string& str) {
        Point point;
        std::regex pattern(R"(\s*(\d+)\s*,\s*(\d+)\s*)");
        std::smatch matches;
        if (std::regex_search(str, matches, pattern) && matches.size() >= 3) {
            point.x = std::stoi(matches[1]);
            point.y = std::stoi(matches[2]);
        }
        return point;
    }

    // 转换为字符串
    std::string toString() const {
        return std::to_string(x) + "," + std::to_string(y);
    }
};

// 矩形区域结构体
struct PIPELINE_API Rect {
    int x1 = 0;
    int y1 = 0;
    int x2 = 0;
    int y2 = 0;

    Rect() = default;
    Rect(int x1, int y1, int x2, int y2) : x1(x1), y1(y1), x2(x2), y2(y2) {}

    // 从字符串解析矩形区域，格式为"x1,y1,x2,y2"
    static Rect fromString(const std::string& str) {
        Rect rect;
        std::regex pattern(R"(\s*(\d+)\s*,\s*(\d+)\s*,\s*(\d+)\s*,\s*(\d+)\s*)");
        std::smatch matches;
        if (std::regex_search(str, matches, pattern) && matches.size() >= 5) {
            rect.x1 = std::stoi(matches[1]);
            rect.y1 = std::stoi(matches[2]);
            rect.x2 = std::stoi(matches[3]);
            rect.y2 = std::stoi(matches[4]);
        }
        return rect;
    }

    // 转换为字符串
    std::string toString() const {
        return std::to_string(x1) + "," + std::to_string(y1) + "," +
               std::to_string(x2) + "," + std::to_string(y2);
    }
};

// 变量值类型，使用std::variant存储不同类型的值
using VariableValue = std::variant<int, std::string, double, bool, Point, Rect>;

// 变量类，表示一个变量
class PIPELINE_API Variable {
public:
    Variable() : m_type(VariableType::Integer), m_value(0) {}
    Variable(VariableType type) : m_type(type) {
        // 根据类型初始化默认值
        switch (type) {
            case VariableType::Integer:
                m_value = 0;
                break;
            case VariableType::String:
                m_value = std::string();
                break;
            case VariableType::Float:
                m_value = 0.0;
                break;
            case VariableType::Boolean:
                m_value = false;
                break;
            case VariableType::Point:
                m_value = Point();
                break;
            case VariableType::Rect:
                m_value = Rect();
                break;
        }
    }

    Variable(VariableType type, VariableValue value) : m_type(type), m_value(std::move(value)) {}

    // 获取变量类型
    VariableType getType() const { return m_type; }

    // 获取变量值
    template<typename T>
    T getValue() const {
        return std::get<T>(m_value);
    }

    // 设置变量值
    template<typename T>
    void setValue(T value) {
        m_value = std::move(value);
    }

    // 获取变量值的字符串表示
    std::string toString() const;

    // 从字符串解析变量值
    bool fromString(const std::string& str);

private:
    VariableType m_type;
    VariableValue m_value;
};

// 变量管理器类，用于管理所有变量
class PIPELINE_API VariableManager {
public:
    VariableManager() = default;
    ~VariableManager() = default;

    // 定义变量
    bool defineVariable(const std::string& name, VariableType type);

    // 定义并初始化变量
    bool defineVariable(const std::string& name, VariableType type, const std::string& value);

    // 获取变量
    std::optional<Variable> getVariable(const std::string& name) const;

    // 设置变量值
    bool setVariable(const std::string& name, const VariableValue& value);

    // 解析变量定义字符串
    bool parseVariableDefinition(const std::string& definition);

    // 解析变量列表
    bool parseVariableList(const std::vector<std::string>& definitions);

    // 计算条件表达式
    bool evaluateCondition(const std::string& condition);

    // 处理日志字符串，替换变量并执行操作
    std::string processLogString(const std::string& logStr);

    // 执行变量操作表达式
    bool executeExpression(const std::string& expression);

private:
    // 变量存储
    std::unordered_map<std::string, Variable> m_variables;

    // 从变量名解析变量类型
    VariableType getTypeFromName(const std::string& name) const;

    // 解析表达式
    VariableValue parseExpression(const std::string& expression, VariableType expectedType);

    // 执行算术表达式
    VariableValue evaluateArithmeticExpression(const std::string& expression, VariableType expectedType);

    // 替换字符串中的变量引用
    std::string replaceVariableReferences(const std::string& str);

    // 执行变量操作
    bool executeVariableOperation(const std::string& operation);
};

} // namespace Pipeline
