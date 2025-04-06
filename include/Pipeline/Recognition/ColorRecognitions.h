#pragma once

#include "Pipeline/Recognition/Recognition.h"
#include <vector>
#include <string>

namespace Pipeline {

// FindColor识别类 - 找色
class PIPELINE_API FindColorRecognition : public Recognition {
public:
    FindColorRecognition();
    virtual RecognitionResult recognize() override;
    virtual bool parseConfig(const nlohmann::json& config) override;

private:
    std::vector<int> m_roi = {0, 0, 0, 0};
    std::vector<int> m_roiOffset = {0, 0, 0, 0};
    std::string m_color;
    double m_similarity = 1.0;
    int m_direction = 0;
};

// FindMultiColor识别类 - 多点找色
class PIPELINE_API FindMultiColorRecognition : public Recognition {
public:
    FindMultiColorRecognition();
    virtual RecognitionResult recognize() override;
    virtual bool parseConfig(const nlohmann::json& config) override;

private:
    std::vector<int> m_roi = {0, 0, 0, 0};
    std::vector<int> m_roiOffset = {0, 0, 0, 0};
    std::string m_firstColor;
    std::string m_offsetColor;
    double m_similarity = 1.0;
    int m_direction = 0;
};

// FindColorList识别类 - 找色列表
class PIPELINE_API FindColorListRecognition : public Recognition {
public:
    FindColorListRecognition();
    virtual RecognitionResult recognize() override;
    virtual bool parseConfig(const nlohmann::json& config) override;

private:
    std::vector<int> m_roi = {0, 0, 0, 0};
    std::vector<int> m_roiOffset = {0, 0, 0, 0};
    std::vector<std::string> m_colorList;
    double m_similarity = 1.0;
    int m_direction = 0;
};

// FindMultiColorList识别类 - 多点找色列表
class PIPELINE_API FindMultiColorListRecognition : public Recognition {
public:
    FindMultiColorListRecognition();
    virtual RecognitionResult recognize() override;
    virtual bool parseConfig(const nlohmann::json& config) override;

private:
    std::vector<int> m_roi = {0, 0, 0, 0};
    std::vector<int> m_roiOffset = {0, 0, 0, 0};
    std::vector<std::pair<std::string, std::string>> m_multiColorList;
    double m_similarity = 1.0;
    int m_direction = 0;
};

} // namespace Pipeline
