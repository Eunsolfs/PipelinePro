#pragma once

#include "Pipeline/Recognition/Recognition.h"
#include <vector>
#include <string>

namespace Pipeline {

// TemplateMatch识别类 - 模板匹配
class PIPELINE_API TemplateMatchRecognition : public Recognition {
public:
    TemplateMatchRecognition();
    virtual RecognitionResult recognize() override;
    virtual bool parseConfig(const nlohmann::json& config) override;

private:
    std::vector<int> m_roi = {0, 0, 0, 0};
    std::vector<int> m_roiOffset = {0, 0, 0, 0};
    std::vector<std::string> m_templates;
    std::vector<double> m_thresholds;
    int m_method = 5; // 默认使用TM_CCOEFF_NORMED
};

} // namespace Pipeline
