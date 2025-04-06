#pragma once

#include "Pipeline/Recognition/Recognition.h"
#include <vector>
#include <string>

namespace Pipeline {

// OCR识别类 - OCR识别
class PIPELINE_API OCRRecognition : public Recognition {
public:
    OCRRecognition();
    virtual RecognitionResult recognize() override;
    virtual bool parseConfig(const nlohmann::json& config) override;
    
    // 批量OCR识别，返回所有结果
    std::vector<RecognitionResult> recognizeBatch();

private:
    // 创建OCR参数
    vision::OcrParams createParams() const;
    
    std::vector<int> m_roi = {0, 0, 0, 0};
    std::vector<int> m_roiOffset = {0, 0, 0, 0};
    std::vector<std::string> m_expected;
    std::vector<std::pair<std::string, std::string>> m_replace;
    std::string m_orderBy = "Horizontal";
    int m_index = 0;
    bool m_onlyRec = false;
    std::string m_model;
};

} // namespace Pipeline
