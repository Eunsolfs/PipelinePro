#pragma once

#include "Pipeline/Recognition/Recognition.h"

namespace Pipeline {

// DirectHit识别类 - 直接命中
class PIPELINE_API DirectHitRecognition : public Recognition {
public:
    DirectHitRecognition();
    virtual RecognitionResult recognize() override;
    virtual bool parseConfig(const nlohmann::json& config) override;
};

} // namespace Pipeline
