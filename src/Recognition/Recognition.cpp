#include "Pipeline/Recognition/Recognition.h"
#include "Pipeline/Recognition/BasicRecognitions.h"
#include "Pipeline/Recognition/ColorRecognitions.h"
#include "Pipeline/Recognition/TemplateRecognitions.h"
#include "Pipeline/Recognition/OcrRecognition.h"
#include "Pipeline/Common.h"

namespace Pipeline {

// 构造函数
Recognition::Recognition(RecognitionType type) : m_type(type) {
}

// 创建识别对象的工厂方法
std::unique_ptr<Recognition> Recognition::create(RecognitionType type, const nlohmann::json& config) {
    std::unique_ptr<Recognition> recognition;
    
    switch (type) {
        case RecognitionType::DirectHit:
            recognition = std::make_unique<DirectHitRecognition>();
            break;
        case RecognitionType::FindColor:
            recognition = std::make_unique<FindColorRecognition>();
            break;
        case RecognitionType::FindMultiColor:
            recognition = std::make_unique<FindMultiColorRecognition>();
            break;
        case RecognitionType::FindColorList:
            recognition = std::make_unique<FindColorListRecognition>();
            break;
        case RecognitionType::FindMultiColorList:
            recognition = std::make_unique<FindMultiColorListRecognition>();
            break;
        case RecognitionType::TemplateMatch:
            recognition = std::make_unique<TemplateMatchRecognition>();
            break;
        case RecognitionType::OCR:
            recognition = std::make_unique<OCRRecognition>();
            break;
        default:
            recognition = std::make_unique<DirectHitRecognition>();
            break;
    }
    
    // 解析配置
    if (recognition && !config.empty()) {
        recognition->parseConfig(config);
    }
    
    return recognition;
}

// 将字符串转换为识别类型
RecognitionType stringToRecognitionType(const std::string& typeStr) {
    if (typeStr == "DirectHit") {
        return RecognitionType::DirectHit;
    } else if (typeStr == "FindColor") {
        return RecognitionType::FindColor;
    } else if (typeStr == "FindMultiColor") {
        return RecognitionType::FindMultiColor;
    } else if (typeStr == "FindColorList") {
        return RecognitionType::FindColorList;
    } else if (typeStr == "FindMultiColorList") {
        return RecognitionType::FindMultiColorList;
    } else if (typeStr == "TemplateMatch") {
        return RecognitionType::TemplateMatch;
    } else if (typeStr == "OCR") {
        return RecognitionType::OCR;
    } else {
        return RecognitionType::DirectHit; // 默认为DirectHit
    }
}

// 将识别类型转换为字符串
std::string recognitionTypeToString(RecognitionType type) {
    switch (type) {
        case RecognitionType::DirectHit:
            return "DirectHit";
        case RecognitionType::FindColor:
            return "FindColor";
        case RecognitionType::FindMultiColor:
            return "FindMultiColor";
        case RecognitionType::FindColorList:
            return "FindColorList";
        case RecognitionType::FindMultiColorList:
            return "FindMultiColorList";
        case RecognitionType::TemplateMatch:
            return "TemplateMatch";
        case RecognitionType::OCR:
            return "OCR";
        default:
            return "Unknown";
    }
}

} // namespace Pipeline
