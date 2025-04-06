#include "Pipeline/Recognition/BasicRecognitions.h"
#include "Pipeline/RecognitionResult.h"

namespace Pipeline {

// DirectHitRecognition实现
DirectHitRecognition::DirectHitRecognition() : Recognition(RecognitionType::DirectHit) {
}

RecognitionResult DirectHitRecognition::recognize() {
    RecognitionResult result;
    result.success = true;
    
    // 如果设置了inverse，则反转结果
    if (m_inverse) {
        result.success = !result.success;
    }
    
    return result;
}

bool DirectHitRecognition::parseConfig(const nlohmann::json& config) {
    // DirectHitRecognition不需要额外的参数
    return true;
}

// AlwaysRecognition实现
AlwaysRecognition::AlwaysRecognition() : Recognition(RecognitionType::Always) {
}

RecognitionResult AlwaysRecognition::recognize() {
    RecognitionResult result;
    result.success = true;
    
    // 如果设置了inverse，则反转结果
    if (m_inverse) {
        result.success = !result.success;
    }
    
    return result;
}

bool AlwaysRecognition::parseConfig(const nlohmann::json& config) {
    // AlwaysRecognition不需要额外的参数
    return true;
}

} // namespace Pipeline
