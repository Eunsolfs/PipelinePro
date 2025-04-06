#include "Pipeline/VariableManager.h"
#include <sstream>
#include <algorithm>
#include <cctype>
#include <stack>
#include <queue>
#include <map>

namespace Pipeline {

// 变量值转字符串
std::string Variable::toString() const {
    std::stringstream ss;

    // 根据类型转换为字符串
    switch (m_type) {
        case VariableType::Integer:
            ss << std::get<int>(m_value);
            break;
        case VariableType::String:
            ss << std::get<std::string>(m_value);
            break;
        case VariableType::Float:
            ss << std::get<double>(m_value);
            break;
        case VariableType::Boolean:
            ss << (std::get<bool>(m_value) ? "true" : "false");
            break;
        case VariableType::Point:
            ss << std::get<Point>(m_value).toString();
            break;
        case VariableType::Rect:
            ss << std::get<Rect>(m_value).toString();
            break;
    }

    return ss.str();
}

// 从字符串解析变量值
bool Variable::fromString(const std::string& str) {
    try {
        // 根据类型解析字符串
        switch (m_type) {
            case VariableType::Integer:
                m_value = std::stoi(str);
                break;
            case VariableType::String:
                m_value = str;
                break;
            case VariableType::Float:
                m_value = std::stod(str);
                break;
            case VariableType::Boolean:
                if (str == "true" || str == "1") {
                    m_value = true;
                } else if (str == "false" || str == "0") {
                    m_value = false;
                } else {
                    return false;
                }
                break;
            case VariableType::Point:
                m_value = Point::fromString(str);
                break;
            case VariableType::Rect:
                m_value = Rect::fromString(str);
                break;
        }
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

// 定义变量
bool VariableManager::defineVariable(const std::string& name, VariableType type) {
    // 检查变量名是否合法
    if (name.empty() || name[0] != '%') {
        return false;
    }

    // 创建变量
    m_variables[name] = Variable(type);
    return true;
}

// 定义并初始化变量
bool VariableManager::defineVariable(const std::string& name, VariableType type, const std::string& value) {
    // 检查变量名是否合法
    if (name.empty() || name[0] != '%') {
        return false;
    }

    // 创建变量
    Variable var(type);

    // 初始化变量值
    if (!var.fromString(value)) {
        return false;
    }

    // 存储变量
    m_variables[name] = std::move(var);
    return true;
}

// 获取变量
std::optional<Variable> VariableManager::getVariable(const std::string& name) const {
    auto it = m_variables.find(name);
    if (it != m_variables.end()) {
        return it->second;
    }
    return std::nullopt;
}

// 设置变量值
bool VariableManager::setVariable(const std::string& name, const VariableValue& value) {
    auto it = m_variables.find(name);
    if (it != m_variables.end()) {
        // 检查类型是否匹配
        if (it->second.getType() == VariableType::Integer && std::holds_alternative<int>(value)) {
            it->second.setValue(std::get<int>(value));
            return true;
        } else if (it->second.getType() == VariableType::String && std::holds_alternative<std::string>(value)) {
            it->second.setValue(std::get<std::string>(value));
            return true;
        } else if (it->second.getType() == VariableType::Float && std::holds_alternative<double>(value)) {
            it->second.setValue(std::get<double>(value));
            return true;
        } else if (it->second.getType() == VariableType::Boolean && std::holds_alternative<bool>(value)) {
            it->second.setValue(std::get<bool>(value));
            return true;
        } else if (it->second.getType() == VariableType::Point && std::holds_alternative<Point>(value)) {
            it->second.setValue(std::get<Point>(value));
            return true;
        } else if (it->second.getType() == VariableType::Rect && std::holds_alternative<Rect>(value)) {
            it->second.setValue(std::get<Rect>(value));
            return true;
        }
    }
    return false;
}

// 从变量名解析变量类型
VariableType VariableManager::getTypeFromName(const std::string& name) const {
    if (name.size() < 2) {
        return VariableType::Integer; // 默认为整数类型
    }

    // 根据前缀确定类型
    switch (name[1]) {
        case 'i':
            return VariableType::Integer;
        case 's':
            return VariableType::String;
        case 'f':
            return VariableType::Float;
        case 'b':
            return VariableType::Boolean;
        case 'p':
            return VariableType::Point;
        case 'r':
            return VariableType::Rect;
        default:
            return VariableType::Integer; // 默认为整数类型
    }
}

// 解析变量定义字符串
bool VariableManager::parseVariableDefinition(const std::string& definition) {
    // 检查变量定义是否合法
    if (definition.empty() || definition[0] != '%') {
        return false;
    }

    // 查找等号
    size_t equalsPos = definition.find('=');

    if (equalsPos == std::string::npos) {
        // 没有等号，只定义变量不初始化
        std::string name = definition;
        VariableType type = getTypeFromName(name);
        return defineVariable(name, type);
    } else {
        // 有等号，定义并初始化变量
        std::string name = definition.substr(0, equalsPos);
        std::string value = definition.substr(equalsPos + 1);
        VariableType type = getTypeFromName(name);
        return defineVariable(name, type, value);
    }
}

// 解析变量列表
bool VariableManager::parseVariableList(const std::vector<std::string>& definitions) {
    bool success = true;

    for (const auto& definition : definitions) {
        if (!parseVariableDefinition(definition)) {
            success = false;
        }
    }

    return success;
}

// 计算条件表达式
bool VariableManager::evaluateCondition(const std::string& condition) {
    // 简单条件解析，支持 <, >, <=, >=, ==, != 操作符

    // 查找比较操作符
    size_t opPos = std::string::npos;
    std::string op;

    // 查找各种操作符
    if ((opPos = condition.find("<=")) != std::string::npos) {
        op = "<=";
    } else if ((opPos = condition.find(">=")) != std::string::npos) {
        op = ">=";
    } else if ((opPos = condition.find("==")) != std::string::npos) {
        op = "==";
    } else if ((opPos = condition.find("!=")) != std::string::npos) {
        op = "!=";
    } else if ((opPos = condition.find("<")) != std::string::npos) {
        op = "<";
    } else if ((opPos = condition.find(">")) != std::string::npos) {
        op = ">";
    } else {
        // 没有操作符，可能是单个变量或表达式
        // 如果是变量，检查其布尔值
        if (condition.size() > 0 && condition[0] == '%') {
            auto var = getVariable(condition);
            if (var && var->getType() == VariableType::Boolean) {
                return var->getValue<bool>();
            } else if (var) {
                // 非布尔类型，尝试转换为布尔值
                // 整数和浮点数：非零为真
                // 字符串：非空为真
                if (var->getType() == VariableType::Integer) {
                    return var->getValue<int>() != 0;
                } else if (var->getType() == VariableType::Float) {
                    return var->getValue<double>() != 0.0;
                } else if (var->getType() == VariableType::String) {
                    return !var->getValue<std::string>().empty();
                }
            }
        }

        // 尝试解析为表达式
        try {
            // 解析为布尔表达式
            VariableValue result = parseExpression(condition, VariableType::Boolean);
            return std::get<bool>(result);
        } catch (const std::exception&) {
            return false;
        }
    }

    // 分割左右操作数
    std::string leftStr = condition.substr(0, opPos);
    std::string rightStr = condition.substr(opPos + op.size());

    // 解析左右操作数
    VariableValue leftVal, rightVal;

    // 如果左操作数是变量
    if (leftStr.size() > 0 && leftStr[0] == '%') {
        auto var = getVariable(leftStr);
        if (!var) {
            return false;
        }

        // 根据变量类型获取值
        if (var->getType() == VariableType::Integer) {
            leftVal = var->getValue<int>();
        } else if (var->getType() == VariableType::String) {
            leftVal = var->getValue<std::string>();
        } else if (var->getType() == VariableType::Float) {
            leftVal = var->getValue<double>();
        } else if (var->getType() == VariableType::Boolean) {
            leftVal = var->getValue<bool>();
        }
    } else {
        // 尝试解析为数字或字符串
        try {
            // 先尝试解析为整数
            leftVal = std::stoi(leftStr);
        } catch (const std::exception&) {
            try {
                // 再尝试解析为浮点数
                leftVal = std::stod(leftStr);
            } catch (const std::exception&) {
                // 最后当作字符串处理
                leftVal = leftStr;
            }
        }
    }

    // 如果右操作数是变量
    if (rightStr.size() > 0 && rightStr[0] == '%') {
        auto var = getVariable(rightStr);
        if (!var) {
            return false;
        }

        // 根据变量类型获取值
        if (var->getType() == VariableType::Integer) {
            rightVal = var->getValue<int>();
        } else if (var->getType() == VariableType::String) {
            rightVal = var->getValue<std::string>();
        } else if (var->getType() == VariableType::Float) {
            rightVal = var->getValue<double>();
        } else if (var->getType() == VariableType::Boolean) {
            rightVal = var->getValue<bool>();
        }
    } else {
        // 尝试解析为数字或字符串
        try {
            // 先尝试解析为整数
            rightVal = std::stoi(rightStr);
        } catch (const std::exception&) {
            try {
                // 再尝试解析为浮点数
                rightVal = std::stod(rightStr);
            } catch (const std::exception&) {
                // 最后当作字符串处理
                rightVal = rightStr;
            }
        }
    }

    // 根据操作符和操作数类型进行比较
    if (std::holds_alternative<int>(leftVal) && std::holds_alternative<int>(rightVal)) {
        // 整数比较
        int left = std::get<int>(leftVal);
        int right = std::get<int>(rightVal);

        if (op == "<") return left < right;
        if (op == ">") return left > right;
        if (op == "<=") return left <= right;
        if (op == ">=") return left >= right;
        if (op == "==") return left == right;
        if (op == "!=") return left != right;
    } else if (std::holds_alternative<double>(leftVal) || std::holds_alternative<double>(rightVal)) {
        // 浮点数比较（如果任一操作数是浮点数）
        double left, right;

        if (std::holds_alternative<int>(leftVal)) {
            left = static_cast<double>(std::get<int>(leftVal));
        } else if (std::holds_alternative<double>(leftVal)) {
            left = std::get<double>(leftVal);
        } else {
            return false; // 类型不兼容
        }

        if (std::holds_alternative<int>(rightVal)) {
            right = static_cast<double>(std::get<int>(rightVal));
        } else if (std::holds_alternative<double>(rightVal)) {
            right = std::get<double>(rightVal);
        } else {
            return false; // 类型不兼容
        }

        if (op == "<") return left < right;
        if (op == ">") return left > right;
        if (op == "<=") return left <= right;
        if (op == ">=") return left >= right;
        if (op == "==") return left == right;
        if (op == "!=") return left != right;
    } else if (std::holds_alternative<std::string>(leftVal) && std::holds_alternative<std::string>(rightVal)) {
        // 字符串比较
        const std::string& left = std::get<std::string>(leftVal);
        const std::string& right = std::get<std::string>(rightVal);

        if (op == "<") return left < right;
        if (op == ">") return left > right;
        if (op == "<=") return left <= right;
        if (op == ">=") return left >= right;
        if (op == "==") return left == right;
        if (op == "!=") return left != right;
    } else if (std::holds_alternative<bool>(leftVal) && std::holds_alternative<bool>(rightVal)) {
        // 布尔值比较
        bool left = std::get<bool>(leftVal);
        bool right = std::get<bool>(rightVal);

        if (op == "==") return left == right;
        if (op == "!=") return left != right;
        // 其他操作符对布尔值没有意义
        return false;
    }

    // 类型不兼容
    return false;
}

// 处理日志字符串，替换变量并执行操作
std::string VariableManager::processLogString(const std::string& logStr) {
    std::string result = logStr;

    // 查找并执行花括号中的变量操作
    std::regex operationRegex("\\{([^{}]+)\\}");
    std::smatch match;
    std::string tempStr = logStr;

    while (std::regex_search(tempStr, match, operationRegex)) {
        std::string operation = match[1].str();
        executeVariableOperation(operation);

        // 从临时字符串中移除已处理的部分
        tempStr = match.suffix();
    }

    // 替换变量引用
    result = replaceVariableReferences(result);

    // 移除花括号中的操作部分
    result = std::regex_replace(result, operationRegex, "");

    return result;
}

// 替换字符串中的变量引用
std::string VariableManager::replaceVariableReferences(const std::string& str) {
    std::string result = str;

    // 查找变量引用（方括号中的变量）
    std::regex varRefRegex("\\[(\\%[^\\[\\]]+)\\]");
    std::smatch match;
    std::string tempStr = str;

    while (std::regex_search(tempStr, match, varRefRegex)) {
        std::string varName = match[1].str();
        auto var = getVariable(varName);

        if (var) {
            // 替换为变量值
            std::string varValue = var->toString();
            result = std::regex_replace(result, std::regex("\\[" + varName + "\\]"), varValue);
        }

        // 从临时字符串中移除已处理的部分
        tempStr = match.suffix();
    }

    return result;
}

// 执行变量操作表达式
bool VariableManager::executeExpression(const std::string& expression) {
    return executeVariableOperation(expression);
}

// 执行变量操作
bool VariableManager::executeVariableOperation(const std::string& operation) {
    // 去除空白字符
    std::string op = operation;
    op.erase(std::remove_if(op.begin(), op.end(), [](unsigned char c) { return std::isspace(c); }), op.end());

    // 检查是否是自增或自减操作
    if (op.size() >= 3 && op[0] == '%') {
        if (op.substr(op.size() - 2) == "++") {
            // 自增操作
            std::string varName = op.substr(0, op.size() - 2);
            auto var = getVariable(varName);

            if (var && var->getType() == VariableType::Integer) {
                int value = var->getValue<int>();
                value++;
                setVariable(varName, value);
                return true;
            }
            return false;
        } else if (op.substr(op.size() - 2) == "--") {
            // 自减操作
            std::string varName = op.substr(0, op.size() - 2);
            auto var = getVariable(varName);

            if (var && var->getType() == VariableType::Integer) {
                int value = var->getValue<int>();
                value--;
                setVariable(varName, value);
                return true;
            }
            return false;
        }
    }

    // 检查是否是赋值操作
    size_t equalsPos = op.find('=');
    if (equalsPos != std::string::npos && equalsPos > 0) {
        std::string varName = op.substr(0, equalsPos);
        std::string exprStr = op.substr(equalsPos + 1);

        // 检查变量是否存在
        auto var = getVariable(varName);
        if (!var) {
            return false;
        }

        // 根据变量类型解析表达式
        VariableType type = var->getType();
        try {
            VariableValue result = parseExpression(exprStr, type);
            return setVariable(varName, result);
        } catch (const std::exception&) {
            return false;
        }
    }

    return false;
}

// 解析表达式
VariableValue VariableManager::parseExpression(const std::string& expression, VariableType expectedType) {
    // 如果表达式是变量引用
    if (expression.size() > 0 && expression[0] == '%') {
        auto var = getVariable(expression);
        if (!var) {
            throw std::runtime_error("Variable not found: " + expression);
        }

        // 根据变量类型返回值
        if (var->getType() == VariableType::Integer) {
            if (expectedType == VariableType::Integer) {
                return var->getValue<int>();
            } else if (expectedType == VariableType::Float) {
                return static_cast<double>(var->getValue<int>());
            } else if (expectedType == VariableType::String) {
                return std::to_string(var->getValue<int>());
            } else if (expectedType == VariableType::Boolean) {
                return var->getValue<int>() != 0;
            }
        } else if (var->getType() == VariableType::String) {
            if (expectedType == VariableType::String) {
                return var->getValue<std::string>();
            } else if (expectedType == VariableType::Boolean) {
                return !var->getValue<std::string>().empty();
            }
        } else if (var->getType() == VariableType::Float) {
            if (expectedType == VariableType::Float) {
                return var->getValue<double>();
            } else if (expectedType == VariableType::Integer) {
                return static_cast<int>(var->getValue<double>());
            } else if (expectedType == VariableType::String) {
                return std::to_string(var->getValue<double>());
            } else if (expectedType == VariableType::Boolean) {
                return var->getValue<double>() != 0.0;
            }
        } else if (var->getType() == VariableType::Boolean) {
            if (expectedType == VariableType::Boolean) {
                return var->getValue<bool>();
            } else if (expectedType == VariableType::Integer) {
                return var->getValue<bool>() ? 1 : 0;
            } else if (expectedType == VariableType::Float) {
                return var->getValue<bool>() ? 1.0 : 0.0;
            } else if (expectedType == VariableType::String) {
                return var->getValue<bool>() ? "true" : "false";
            }
        }

        throw std::runtime_error("Type mismatch in expression");
    }

    // 如果是算术表达式
    if (expression.find_first_of("+-*/") != std::string::npos) {
        return evaluateArithmeticExpression(expression, expectedType);
    }

    // 如果是常量
    if (expectedType == VariableType::Integer) {
        try {
            return std::stoi(expression);
        } catch (const std::exception&) {
            throw std::runtime_error("Invalid integer expression: " + expression);
        }
    } else if (expectedType == VariableType::Float) {
        try {
            return std::stod(expression);
        } catch (const std::exception&) {
            throw std::runtime_error("Invalid float expression: " + expression);
        }
    } else if (expectedType == VariableType::String) {
        return expression;
    } else if (expectedType == VariableType::Boolean) {
        if (expression == "true" || expression == "1") {
            return true;
        } else if (expression == "false" || expression == "0") {
            return false;
        } else {
            throw std::runtime_error("Invalid boolean expression: " + expression);
        }
    }

    throw std::runtime_error("Unknown expression type");
}

// 执行算术表达式
VariableValue VariableManager::evaluateArithmeticExpression(const std::string& expression, VariableType expectedType) {
    // 这里实现一个简单的算术表达式解析器
    // 支持 +, -, *, / 操作符和变量引用

    // 首先将表达式中的变量替换为其值
    std::string expr = expression;
    std::regex varRegex("\\%[a-zA-Z0-9_]+");
    std::smatch match;

    while (std::regex_search(expr, match, varRegex)) {
        std::string varName = match[0].str();
        auto var = getVariable(varName);

        if (!var) {
            throw std::runtime_error("Variable not found: " + varName);
        }

        // 根据变量类型替换为其值
        std::string replacement;
        if (var->getType() == VariableType::Integer) {
            replacement = std::to_string(var->getValue<int>());
        } else if (var->getType() == VariableType::Float) {
            replacement = std::to_string(var->getValue<double>());
        } else if (var->getType() == VariableType::String) {
            // 字符串不能参与算术运算
            throw std::runtime_error("String variable in arithmetic expression: " + varName);
        } else if (var->getType() == VariableType::Boolean) {
            replacement = var->getValue<bool>() ? "1" : "0";
        }

        // 替换变量
        expr = std::regex_replace(expr, std::regex(varName), replacement);
    }

    // 现在expr中只包含数字和操作符，可以使用表达式求值算法
    // 这里使用一个简化版的表达式求值算法，只支持基本的四则运算

    // 将中缀表达式转换为后缀表达式（逆波兰表示法）
    std::queue<std::string> output;
    std::stack<char> operators;

    // 定义操作符优先级
    std::map<char, int> precedence = {
        {'+', 1},
        {'-', 1},
        {'*', 2},
        {'/', 2}
    };

    // 解析表达式
    for (size_t i = 0; i < expr.size(); ++i) {
        char c = expr[i];

        if (std::isdigit(c) || c == '.') {
            // 数字或小数点，解析为数值
            std::string number;
            while (i < expr.size() && (std::isdigit(expr[i]) || expr[i] == '.')) {
                number += expr[i++];
            }
            --i; // 回退一个字符，因为循环会自增
            output.push(number);
        } else if (c == '+' || c == '-' || c == '*' || c == '/') {
            // 操作符
            while (!operators.empty() && precedence[operators.top()] >= precedence[c]) {
                output.push(std::string(1, operators.top()));
                operators.pop();
            }
            operators.push(c);
        }
    }

    // 将剩余的操作符加入输出队列
    while (!operators.empty()) {
        output.push(std::string(1, operators.top()));
        operators.pop();
    }

    // 计算后缀表达式
    std::stack<double> values;

    while (!output.empty()) {
        std::string token = output.front();
        output.pop();

        if (token.size() == 1 && (token[0] == '+' || token[0] == '-' || token[0] == '*' || token[0] == '/')) {
            // 操作符
            if (values.size() < 2) {
                throw std::runtime_error("Invalid expression: not enough operands");
            }

            double b = values.top();
            values.pop();
            double a = values.top();
            values.pop();

            switch (token[0]) {
                case '+':
                    values.push(a + b);
                    break;
                case '-':
                    values.push(a - b);
                    break;
                case '*':
                    values.push(a * b);
                    break;
                case '/':
                    if (b == 0) {
                        throw std::runtime_error("Division by zero");
                    }
                    values.push(a / b);
                    break;
            }
        } else {
            // 数值
            try {
                values.push(std::stod(token));
            } catch (const std::exception&) {
                throw std::runtime_error("Invalid number in expression: " + token);
            }
        }
    }

    if (values.size() != 1) {
        throw std::runtime_error("Invalid expression: too many operands");
    }

    double result = values.top();

    // 根据期望类型返回结果
    if (expectedType == VariableType::Integer) {
        return static_cast<int>(result);
    } else if (expectedType == VariableType::Float) {
        return result;
    } else if (expectedType == VariableType::String) {
        return std::to_string(result);
    } else if (expectedType == VariableType::Boolean) {
        return result != 0.0;
    }

    throw std::runtime_error("Unknown expected type");
}

} // namespace Pipeline
