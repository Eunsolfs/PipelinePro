#pragma once

#define PIPELINE_EXPORTS

// DLL导出宏定义
#ifdef PIPELINE_EXPORTS
#define PIPELINE_API __declspec(dllexport)
#else
#define PIPELINE_API __declspec(dllimport)
#endif

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <optional>
#include <coroutine>
#include <future>
#include <variant>
#include <any>
#include <chrono>
#include <thread>
#include <regex>
#include <filesystem>
#include <iostream>
#include <sstream>

// 前向声明
namespace nlohmann {
    template<typename T>
    class basic_json;
    
    using json = basic_json<>;
}

namespace Pipeline {

// 识别类型枚举
enum class RecognitionType {
    DirectHit,
    TemplateMatch,
    FindColor,
    OCR,
    FindMultiColor,
    FindColorList,
    FindMultiColorList
};

// 动作类型枚举
enum class ActionType {
    DoNothing,
    Click,
    Swipe,
    Key,
    Text,
    StartApp,
    StopApp,
    StopTask,
    Command
};

// 矩形区域结构体
struct Rect {
    int x1 = 0;
    int y1 = 0;
    int x2 = 0;
    int y2 = 0;
    int width = 0;
    int height = 0;
    
    Rect() = default;
    Rect(int x1, int y1, int x2, int y2) : x1(x1), y1(y1), x2(x2), y2(y2) {
        width = std::abs(width);
        height = std::abs(height);
    }
    
    std::string toString() const {
        std::stringstream ss;
        ss << "(" << x1 << "," << y1 << "," << x2 << "," << y2 << ")";
        return ss.str();
    }
    
    int width() const { return width; }
    int height() const { return height; }
};

// 识别结果结构体
struct RecognitionResult {
    bool success = false;
    Rect box;
    double score = 0.0;
    std::string text;
    
    // 特定识别类型可能需要的额外数据
    std::any additionalData;
    
    operator bool() const { return success; }
};

// 字符串转换为枚举类型的辅助函数
PIPELINE_API RecognitionType stringToRecognitionType(const std::string& str);
PIPELINE_API ActionType stringToActionType(const std::string& str);
PIPELINE_API std::string recognitionTypeToString(RecognitionType type);
PIPELINE_API std::string actionTypeToString(ActionType type);

} // namespace Pipeline
