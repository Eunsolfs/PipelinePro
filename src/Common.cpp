#include "Pipeline/Common.h"
#include <unordered_map>
#include <algorithm>

namespace Pipeline {

// 字符串到识别类型的映射
static const std::unordered_map<std::string, RecognitionType> s_recognitionTypeMap = {
    {"DirectHit", RecognitionType::DirectHit},
    {"TemplateMatch", RecognitionType::TemplateMatch},
    {"findcolor", RecognitionType::FindColor},
    {"OCR", RecognitionType::OCR},
    {"findMultiColor", RecognitionType::FindMultiColor},
    {"findcolorlist", RecognitionType::FindColorList},
    {"findMultiColorlist", RecognitionType::FindMultiColorList}
};

// 字符串到动作类型的映射
static const std::unordered_map<std::string, ActionType> s_actionTypeMap = {
    {"DoNothing", ActionType::DoNothing},
    {"Click", ActionType::Click},
    {"Swipe", ActionType::Swipe},
    {"Key", ActionType::Key},
    {"Text", ActionType::Text},
    {"StartApp", ActionType::StartApp},
    {"StopApp", ActionType::StopApp},
    {"StopTask", ActionType::StopTask},
    {"Command", ActionType::Command}
};

// 识别类型到字符串的映射
static const std::unordered_map<RecognitionType, std::string> s_recognitionTypeStringMap = {
    {RecognitionType::DirectHit, "DirectHit"},
    {RecognitionType::TemplateMatch, "TemplateMatch"},
    {RecognitionType::FindColor, "findcolor"},
    {RecognitionType::OCR, "OCR"},
    {RecognitionType::FindMultiColor, "findMultiColor"},
    {RecognitionType::FindColorList, "findcolorlist"},
    {RecognitionType::FindMultiColorList, "findMultiColorlist"}
};

// 动作类型到字符串的映射
static const std::unordered_map<ActionType, std::string> s_actionTypeStringMap = {
    {ActionType::DoNothing, "DoNothing"},
    {ActionType::Click, "Click"},
    {ActionType::Swipe, "Swipe"},
    {ActionType::Key, "Key"},
    {ActionType::Text, "Text"},
    {ActionType::StartApp, "StartApp"},
    {ActionType::StopApp, "StopApp"},
    {ActionType::StopTask, "StopTask"},
    {ActionType::Command, "Command"}
};

RecognitionType stringToRecognitionType(const std::string& str) {
    auto it = s_recognitionTypeMap.find(str);
    if (it != s_recognitionTypeMap.end()) {
        return it->second;
    }
    return RecognitionType::DirectHit; // 默认值
}

ActionType stringToActionType(const std::string& str) {
    auto it = s_actionTypeMap.find(str);
    if (it != s_actionTypeMap.end()) {
        return it->second;
    }
    return ActionType::DoNothing; // 默认值
}

std::string recognitionTypeToString(RecognitionType type) {
    auto it = s_recognitionTypeStringMap.find(type);
    if (it != s_recognitionTypeStringMap.end()) {
        return it->second;
    }
    return "DirectHit"; // 默认值
}

std::string actionTypeToString(ActionType type) {
    auto it = s_actionTypeStringMap.find(type);
    if (it != s_actionTypeStringMap.end()) {
        return it->second;
    }
    return "DoNothing"; // 默认值
}

} // namespace Pipeline
