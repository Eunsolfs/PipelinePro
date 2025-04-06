#include "Pipeline/Action.h"
#include <nlohmann/json.hpp>

namespace Pipeline {

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

// ClickAction实现
ClickAction::ClickAction() : Action(ActionType::Click) {
}

bool ClickAction::parseConfig(const nlohmann::json& config) {
    // 解析目标
    if (config.contains("target")) {
        if (config["target"].is_boolean()) {
            m_target = config["target"].get<bool>();
        } else if (config["target"].is_string()) {
            m_target = config["target"].get<std::string>();
        } else if (config["target"].is_array()) {
            m_target = config["target"].get<std::vector<int>>();
        }
    }

    // 解析目标偏移
    if (config.contains("target_offset")) {
        m_targetOffset = config["target_offset"].get<std::vector<int>>();
    }

    return true;
}

bool ClickAction::execute(const RecognitionResult& result) {
    // 这里应该实现实际的点击操作
    // 由于实际的点击操作不在本库的范围内，这里只是一个示例实现

    // 计算点击位置
    int clickX = 0;
    int clickY = 0;
    bool positionFound = false;

    // 根据m_target的类型确定点击位置
    if (std::holds_alternative<bool>(m_target)) {
        if (std::get<bool>(m_target) && result.success) {
            // 点击识别结果的位置
            clickX = result.box.x + result.box.width / 2;
            clickY = result.box.y + result.box.height / 2;
            positionFound = true;
        }
    } else if (std::holds_alternative<std::string>(m_target)) {
        std::string targetStr = std::get<std::string>(m_target);

        // 检查是否包含变量表达式
        if (m_variableManager && (targetStr.find("%") != std::string::npos ||
                                 targetStr.find("[") != std::string::npos ||
                                 targetStr.find("{") != std::string::npos)) {
            // 处理变量表达式
            std::string processedStr = m_variableManager->processLogString(targetStr);

            // 尝试解析为坐标
            std::regex coordPattern(R"(\s*(\d+)\s*,\s*(\d+)\s*)");
            std::smatch matches;
            if (std::regex_search(processedStr, matches, coordPattern) && matches.size() >= 3) {
                clickX = std::stoi(matches[1]);
                clickY = std::stoi(matches[2]);
                positionFound = true;
            }

            // 尝试直接获取点坐标变量
            if (!positionFound && targetStr.find("%p") != std::string::npos) {
                // 提取变量名
                std::regex varPattern(R"(%p[a-zA-Z0-9_]+)");
                std::smatch varMatches;
                if (std::regex_search(targetStr, varMatches, varPattern)) {
                    std::string varName = varMatches[0];
                    auto pointVar = m_variableManager->getVariable(varName);
                    if (pointVar && std::holds_alternative<Point>(*pointVar)) {
                        const Point& point = std::get<Point>(*pointVar);
                        clickX = point.x;
                        clickY = point.y;
                        positionFound = true;
                    }
                }
            }
        } else {
            // 尝试作为节点名处理
            // 在实际实现中，这里应该查找指定节点的识别结果
            // 这里仅作为示例，使用当前结果
            if (result.success) {
                clickX = result.box.x + result.box.width / 2;
                clickY = result.box.y + result.box.height / 2;
                positionFound = true;
            }
        }
    } else if (std::holds_alternative<std::vector<int>>(m_target)) {
        const auto& coords = std::get<std::vector<int>>(m_target);
        if (coords.size() >= 4) {
            // 在指定区域内随机选择一点
            int x = coords[0];
            int y = coords[1];
            int width = coords[2];
            int height = coords[3];

            // 如果宽高为0，则使用固定点
            if (width == 0 && height == 0) {
                clickX = x;
                clickY = y;
            } else {
                // 随机选择区域内的一点
                clickX = x + (width > 0 ? (rand() % width) : 0);
                clickY = y + (height > 0 ? (rand() % height) : 0);
            }
            positionFound = true;
        } else if (coords.size() >= 2) {
            // 只有x和y坐标
            clickX = coords[0];
            clickY = coords[1];
            positionFound = true;
        }
    }

    // 如果找到了点击位置，应用偏移
    if (positionFound && m_targetOffset.size() >= 2) {
        clickX += m_targetOffset[0];
        clickY += m_targetOffset[1];

        // 如果有随机偏移范围
        if (m_targetOffset.size() >= 4) {
            int offsetWidth = m_targetOffset[2];
            int offsetHeight = m_targetOffset[3];

            if (offsetWidth > 0) {
                clickX += rand() % offsetWidth;
            }

            if (offsetHeight > 0) {
                clickY += rand() % offsetHeight;
            }
        }

        // 执行点击
        // 在实际实现中，这里应该调用实际的点击函数
        std::cout << "Clicking at: " << clickX << ", " << clickY << std::endl;
        return true; // 假设点击成功
    }

    return false; // 如果没有找到点击位置，返回失败
}

// SwipeAction实现
SwipeAction::SwipeAction() : Action(ActionType::Swipe) {
}

bool SwipeAction::parseConfig(const nlohmann::json& config) {
    // 解析起点
    if (config.contains("begin")) {
        if (config["begin"].is_boolean()) {
            m_begin = config["begin"].get<bool>();
        } else if (config["begin"].is_string()) {
            m_begin = config["begin"].get<std::string>();
        } else if (config["begin"].is_array()) {
            m_begin = config["begin"].get<std::vector<int>>();
        }
    }

    // 解析起点偏移
    if (config.contains("begin_offset")) {
        m_beginOffset = config["begin_offset"].get<std::vector<int>>();
    }

    // 解析终点
    if (config.contains("end")) {
        if (config["end"].is_boolean()) {
            m_end = config["end"].get<bool>();
        } else if (config["end"].is_string()) {
            m_end = config["end"].get<std::string>();
        } else if (config["end"].is_array()) {
            m_end = config["end"].get<std::vector<int>>();
        }
    }

    // 解析终点偏移
    if (config.contains("end_offset")) {
        m_endOffset = config["end_offset"].get<std::vector<int>>();
    }

    // 解析持续时间
    if (config.contains("duration")) {
        m_duration = config["duration"].get<uint32_t>();
    }

    return true;
}

bool SwipeAction::execute(const RecognitionResult& result) {
    // 这里应该实现实际的滑动操作
    // 由于实际的滑动操作不在本库的范围内，这里只是一个示例实现

    // 计算起点和终点
    int beginX = 0, beginY = 0;
    int endX = 0, endY = 0;
    bool beginFound = false, endFound = false;

    // 处理起点
    if (std::holds_alternative<bool>(m_begin)) {
        if (std::get<bool>(m_begin) && result.success) {
            // 使用识别结果的位置作为起点
            beginX = result.box.x + result.box.width / 2;
            beginY = result.box.y + result.box.height / 2;
            beginFound = true;
        }
    } else if (std::holds_alternative<std::string>(m_begin)) {
        std::string beginStr = std::get<std::string>(m_begin);

        // 检查是否包含变量表达式
        if (m_variableManager && (beginStr.find("%") != std::string::npos ||
                                 beginStr.find("[") != std::string::npos ||
                                 beginStr.find("{") != std::string::npos)) {
            // 处理变量表达式
            std::string processedStr = m_variableManager->processLogString(beginStr);

            // 尝试解析为坐标
            std::regex coordPattern(R"(\s*(\d+)\s*,\s*(\d+)\s*)");
            std::smatch matches;
            if (std::regex_search(processedStr, matches, coordPattern) && matches.size() >= 3) {
                beginX = std::stoi(matches[1]);
                beginY = std::stoi(matches[2]);
                beginFound = true;
            }

            // 尝试直接获取点坐标变量
            if (!beginFound && beginStr.find("%p") != std::string::npos) {
                // 提取变量名
                std::regex varPattern(R"(%p[a-zA-Z0-9_]+)");
                std::smatch varMatches;
                if (std::regex_search(beginStr, varMatches, varPattern)) {
                    std::string varName = varMatches[0];
                    auto pointVar = m_variableManager->getVariable(varName);
                    if (pointVar && std::holds_alternative<Point>(*pointVar)) {
                        const Point& point = std::get<Point>(*pointVar);
                        beginX = point.x;
                        beginY = point.y;
                        beginFound = true;
                    }
                }
            }
        } else {
            // 尝试作为节点名处理
            // 在实际实现中，这里应该查找指定节点的识别结果
            // 这里仅作为示例，使用当前结果
            if (result.success) {
                beginX = result.box.x + result.box.width / 2;
                beginY = result.box.y + result.box.height / 2;
                beginFound = true;
            }
        }
    } else if (std::holds_alternative<std::vector<int>>(m_begin)) {
        const auto& coords = std::get<std::vector<int>>(m_begin);
        if (coords.size() >= 4) {
            // 在指定区域内随机选择一点
            int x = coords[0];
            int y = coords[1];
            int width = coords[2];
            int height = coords[3];

            // 如果宽高为0，则使用固定点
            if (width == 0 && height == 0) {
                beginX = x;
                beginY = y;
            } else {
                // 随机选择区域内的一点
                beginX = x + (width > 0 ? (rand() % width) : 0);
                beginY = y + (height > 0 ? (rand() % height) : 0);
            }
            beginFound = true;
        } else if (coords.size() >= 2) {
            // 只有x和y坐标
            beginX = coords[0];
            beginY = coords[1];
            beginFound = true;
        }
    }

    // 处理终点
    if (std::holds_alternative<bool>(m_end)) {
        if (std::get<bool>(m_end) && result.success) {
            // 使用识别结果的位置作为终点
            endX = result.box.x + result.box.width / 2;
            endY = result.box.y + result.box.height / 2;
            endFound = true;
        }
    } else if (std::holds_alternative<std::string>(m_end)) {
        std::string endStr = std::get<std::string>(m_end);

        // 检查是否包含变量表达式
        if (m_variableManager && (endStr.find("%") != std::string::npos ||
                                endStr.find("[") != std::string::npos ||
                                endStr.find("{") != std::string::npos)) {
            // 处理变量表达式
            std::string processedStr = m_variableManager->processLogString(endStr);

            // 尝试解析为坐标
            std::regex coordPattern(R"(\s*(\d+)\s*,\s*(\d+)\s*)");
            std::smatch matches;
            if (std::regex_search(processedStr, matches, coordPattern) && matches.size() >= 3) {
                endX = std::stoi(matches[1]);
                endY = std::stoi(matches[2]);
                endFound = true;
            }

            // 尝试直接获取点坐标变量
            if (!endFound && endStr.find("%p") != std::string::npos) {
                // 提取变量名
                std::regex varPattern(R"(%p[a-zA-Z0-9_]+)");
                std::smatch varMatches;
                if (std::regex_search(endStr, varMatches, varPattern)) {
                    std::string varName = varMatches[0];
                    auto pointVar = m_variableManager->getVariable(varName);
                    if (pointVar && std::holds_alternative<Point>(*pointVar)) {
                        const Point& point = std::get<Point>(*pointVar);
                        endX = point.x;
                        endY = point.y;
                        endFound = true;
                    }
                }
            }

            // 尝试直接获取矩形区域变量
            if (!endFound && endStr.find("%r") != std::string::npos) {
                // 提取变量名
                std::regex varPattern(R"(%r[a-zA-Z0-9_]+)");
                std::smatch varMatches;
                if (std::regex_search(endStr, varMatches, varPattern)) {
                    std::string varName = varMatches[0];
                    auto rectVar = m_variableManager->getVariable(varName);
                    if (rectVar && std::holds_alternative<Rect>(*rectVar)) {
                        const Rect& rect = std::get<Rect>(*rectVar);
                        // 使用矩形的右下角作为终点
                        endX = rect.x2;
                        endY = rect.y2;
                        endFound = true;
                    }
                }
            }
        } else {
            // 尝试作为节点名处理
            // 在实际实现中，这里应该查找指定节点的识别结果
            // 这里仅作为示例，使用当前结果
            if (result.success) {
                endX = result.box.x + result.box.width / 2;
                endY = result.box.y + result.box.height / 2;
                endFound = true;
            }
        }
    } else if (std::holds_alternative<std::vector<int>>(m_end)) {
        const auto& coords = std::get<std::vector<int>>(m_end);
        if (coords.size() >= 4) {
            // 在指定区域内随机选择一点
            int x = coords[0];
            int y = coords[1];
            int width = coords[2];
            int height = coords[3];

            // 如果宽高为0，则使用固定点
            if (width == 0 && height == 0) {
                endX = x;
                endY = y;
            } else {
                // 随机选择区域内的一点
                endX = x + (width > 0 ? (rand() % width) : 0);
                endY = y + (height > 0 ? (rand() % height) : 0);
            }
            endFound = true;
        } else if (coords.size() >= 2) {
            // 只有x和y坐标
            endX = coords[0];
            endY = coords[1];
            endFound = true;
        }
    }

    // 应用偏移
    if (beginFound && m_beginOffset.size() >= 2) {
        beginX += m_beginOffset[0];
        beginY += m_beginOffset[1];

        // 如果有随机偏移范围
        if (m_beginOffset.size() >= 4) {
            int offsetWidth = m_beginOffset[2];
            int offsetHeight = m_beginOffset[3];

            if (offsetWidth > 0) {
                beginX += rand() % offsetWidth;
            }

            if (offsetHeight > 0) {
                beginY += rand() % offsetHeight;
            }
        }
    }

    if (endFound && m_endOffset.size() >= 2) {
        endX += m_endOffset[0];
        endY += m_endOffset[1];

        // 如果有随机偏移范围
        if (m_endOffset.size() >= 4) {
            int offsetWidth = m_endOffset[2];
            int offsetHeight = m_endOffset[3];

            if (offsetWidth > 0) {
                endX += rand() % offsetWidth;
            }

            if (offsetHeight > 0) {
                endY += rand() % offsetHeight;
            }
        }
    }

    // 如果起点和终点都找到了，执行滑动
    if (beginFound && endFound) {
        // 执行滑动
        // 在实际实现中，这里应该调用实际的滑动函数，并传入m_duration
        std::cout << "Swiping from: (" << beginX << ", " << beginY << ") to (" << endX << ", " << endY << ") with duration: " << m_duration << "ms" << std::endl;

        // 如果有变量管理器，可以将起点和终点保存到变量中
        if (m_variableManager) {
            // 创建点坐标变量
            Point beginPoint(beginX, beginY);
            Point endPoint(endX, endY);

            // 创建矩形区域变量，包含起点和终点
            int left = std::min(beginX, endX);
            int top = std::min(beginY, endY);
            int right = std::max(beginX, endX);
            int bottom = std::max(beginY, endY);
            Rect swipeRect(left, top, right, bottom);

            // 将点坐标和矩形区域保存到变量中
            // 注意：这里仅作为示例，实际应用中可能需要根据需求决定是否保存这些变量
            m_variableManager->setVariable("%pLastSwipeBegin", beginPoint);
            m_variableManager->setVariable("%pLastSwipeEnd", endPoint);
            m_variableManager->setVariable("%rLastSwipeArea", swipeRect);
        }

        return true; // 假设滑动成功
    }

    return false; // 如果没有找到起点或终点，返回失败
}

// KeyAction实现
KeyAction::KeyAction() : Action(ActionType::Key) {
}

bool KeyAction::parseConfig(const nlohmann::json& config) {
    // 解析按键
    if (config.contains("key")) {
        if (config["key"].is_number()) {
            m_keys.push_back(config["key"].get<int>());
        } else if (config["key"].is_array()) {
            m_keys = config["key"].get<std::vector<int>>();
        }
    }

    return true;
}

bool KeyAction::execute(const RecognitionResult& result) {
    // 这里应该实现实际的按键操作
    // 由于实际的按键操作不在本库的范围内，这里只是一个示例实现

    // 执行按键
    // 在实际实现中，这里应该遍历m_keys并调用实际的按键函数

    return true; // 假设按键成功
}

// TextAction实现
TextAction::TextAction() : Action(ActionType::Text) {
}

bool TextAction::parseConfig(const nlohmann::json& config) {
    // 解析输入文本
    if (config.contains("input_text")) {
        m_inputText = config["input_text"].get<std::string>();
    }

    return true;
}

bool TextAction::execute(const RecognitionResult& result) {
    // 这里应该实现实际的文本输入操作
    // 由于实际的文本输入操作不在本库的范围内，这里只是一个示例实现

    // 执行文本输入
    // 在实际实现中，这里应该调用实际的文本输入函数，并传入m_inputText

    return true; // 假设文本输入成功
}

// StartAppAction实现
StartAppAction::StartAppAction() : Action(ActionType::StartApp) {
}

bool StartAppAction::parseConfig(const nlohmann::json& config) {
    // 解析包名
    if (config.contains("package")) {
        m_package = config["package"].get<std::string>();
    }

    return true;
}

bool StartAppAction::execute(const RecognitionResult& result) {
    // 这里应该实现实际的启动应用操作
    // 由于实际的启动应用操作不在本库的范围内，这里只是一个示例实现

    // 执行启动应用
    // 在实际实现中，这里应该调用实际的启动应用函数，并传入m_package

    return true; // 假设启动应用成功
}

// StopAppAction实现
StopAppAction::StopAppAction() : Action(ActionType::StopApp) {
}

bool StopAppAction::parseConfig(const nlohmann::json& config) {
    // 解析包名
    if (config.contains("package")) {
        m_package = config["package"].get<std::string>();
    }

    return true;
}

bool StopAppAction::execute(const RecognitionResult& result) {
    // 这里应该实现实际的停止应用操作
    // 由于实际的停止应用操作不在本库的范围内，这里只是一个示例实现

    // 执行停止应用
    // 在实际实现中，这里应该调用实际的停止应用函数，并传入m_package

    return true; // 假设停止应用成功
}

// StopTaskAction实现
StopTaskAction::StopTaskAction() : Action(ActionType::StopTask) {
}

bool StopTaskAction::parseConfig(const nlohmann::json& config) {
    // StopTaskAction不需要额外的参数
    return true;
}

bool StopTaskAction::execute(const RecognitionResult& result) {
    // 获取Pipeline实例
    auto pipeline = Pipeline::getInstance();
    if (pipeline) {
        // 触发任务停止事件
        pipeline->triggerTaskStop(pipeline->getCurrentNodeName(), "Task stopped by StopTaskAction");

        // 停止流水线执行
        pipeline->stop();
    }

    return true; // 停止任务成功
}

// CommandAction实现
CommandAction::CommandAction() : Action(ActionType::Command) {
}

bool CommandAction::parseConfig(const nlohmann::json& config) {
    // 解析执行程序
    if (config.contains("exec")) {
        m_exec = config["exec"].get<std::string>();
    }

    // 解析参数
    if (config.contains("args")) {
        m_args = config["args"].get<std::vector<std::string>>();
    }

    // 解析是否分离
    if (config.contains("detach")) {
        m_detach = config["detach"].get<bool>();
    }

    return true;
}

bool CommandAction::execute(const RecognitionResult& result) {
    // 这里应该实现实际的执行命令操作
    // 由于实际的执行命令操作不在本库的范围内，这里只是一个示例实现

    // 执行命令
    // 在实际实现中，这里应该调用实际的执行命令函数，并传入m_exec、m_args和m_detach

    return true; // 假设执行命令成功
}

} // namespace Pipeline
