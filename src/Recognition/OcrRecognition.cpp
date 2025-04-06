#include "Pipeline/Recognition/OcrRecognition.h"
#include "Pipeline/Common.h"
#include <vision/vision.h>
#include <iostream>
#include <vector>

namespace Pipeline {

// OCRRecognition实现
OCRRecognition::OCRRecognition() : Recognition(RecognitionType::OCR) {
}

bool OCRRecognition::parseConfig(const nlohmann::json& config) {
    // 解析ROI
    if (config.contains("roi")) {
        m_roi = config["roi"].get<std::vector<int>>();
    }

    // 解析ROI偏移
    if (config.contains("roi_offset")) {
        m_roiOffset = config["roi_offset"].get<std::vector<int>>();
    }

    // 解析期望的结果
    if (config.contains("expected")) {
        if (config["expected"].is_string()) {
            m_expected.push_back(config["expected"].get<std::string>());
        } else if (config["expected"].is_array()) {
            m_expected = config["expected"].get<std::vector<std::string>>();
        }
    }

    // 解析替换规则
    if (config.contains("replace") && config["replace"].is_array()) {
        for (const auto& item : config["replace"]) {
            if (item.is_array() && item.size() >= 2) {
                std::string pattern = item[0];
                std::string replacement = item[1];
                m_replace.push_back(std::make_pair(pattern, replacement));
            }
        }
    }

    // 解析排序方式
    if (config.contains("orderBy")) {
        m_orderBy = config["orderBy"].get<std::string>();
    }

    // 解析索引
    if (config.contains("index")) {
        m_index = config["index"].get<int>();
    }

    // 解析是否仅识别
    if (config.contains("onlyRec")) {
        m_onlyRec = config["onlyRec"].get<bool>();
    }

    // 解析模型
    if (config.contains("model")) {
        m_model = config["model"].get<std::string>();
    }

    return true;
}

// 创建OCR参数
vision::OcrParams OCRRecognition::createParams() const {
    vision::OcrParams params;

    // 设置ROI
    if (m_roi.size() >= 4) {
        params.roi = vision::Rect(m_roi[0], m_roi[1], m_roi[2], m_roi[3]);

        // 应用ROI偏移
        if (m_roiOffset.size() >= 4) {
            params.roi.x1 += m_roiOffset[0];
            params.roi.y1 += m_roiOffset[1];
            params.roi.x2 += m_roiOffset[2];
            params.roi.y2 += m_roiOffset[3];
        }
    } else {
        // 使用全屏
        params.roi = vision::Rect(0, 0, 1920, 1080); // 默认全屏分辨率
    }

    // 设置期望的结果
    params.expected = m_expected;

    // 设置替换规则
    params.replace = m_replace;

    // 设置排序方式
    params.orderBy = m_orderBy;

    // 设置索引
    params.index = m_index;

    // 设置是否仅识别
    params.onlyRec = m_onlyRec;

    // 设置模型
    params.model = m_model;

    return params;
}

RecognitionResult OCRRecognition::recognize() {
    // 使用vision库进行OCR识别

    // 创建识别参数
    vision::OcrParams params = createParams();

    // 初始化结果
    RecognitionResult result;

    try {
        // 先尝试使用新的批量OCR功能
        auto visionResults = vision::VisionEngine::ocrBatch(params);

        if (!visionResults.empty()) {
            // 根据索引选择结果
            int index = m_index;
            if (index < 0) {
                index = visionResults.size() + index;
            }

            if (index >= 0 && index < static_cast<int>(visionResults.size())) {
                auto& visionResult = visionResults[index];

                // 转换结果
                result.success = visionResult.success;
                result.box.x = visionResult.box.x1;
                result.box.y = visionResult.box.y1;
                result.box.width = visionResult.box.width();
                result.box.height = visionResult.box.height();
                result.score = visionResult.score;
                result.text = visionResult.text;
            }
        }
    } catch (const std::exception& e) {
        // 如果新方法失败，回退到旧方法
        auto visionResult = vision::VisionEngine::ocr(params);

        // 转换结果
        result.success = visionResult.success;
        result.box.x = visionResult.box.x1;
        result.box.y = visionResult.box.y1;
        result.box.width = visionResult.box.width();
        result.box.height = visionResult.box.height();
        result.score = visionResult.score;
        result.text = visionResult.text;
    }

    // 如果设置了inverse，则反转结果
    if (m_inverse) {
        result.success = !result.success;
    }

    return result;
}

// 批量OCR识别，返回所有结果
std::vector<RecognitionResult> OCRRecognition::recognizeBatch() {
    // 创建识别参数
    vision::OcrParams params = createParams();

    // 初始化结果列表
    std::vector<RecognitionResult> results;

    try {
        // 执行批量OCR识别
        auto visionResults = vision::VisionEngine::ocrBatch(params);

        // 转换结果
        for (const auto& visionResult : visionResults) {
            RecognitionResult result;
            result.success = visionResult.success;
            result.box.x = visionResult.box.x1;
            result.box.y = visionResult.box.y1;
            result.box.width = visionResult.box.width();
            result.box.height = visionResult.box.height();
            result.score = visionResult.score;
            result.text = visionResult.text;

            // 如果设置了inverse，则反转结果
            if (m_inverse) {
                result.success = !result.success;
            }

            results.push_back(result);
        }
    } catch (const std::exception& e) {
        // 如果新方法失败，回退到旧方法
        auto visionResult = vision::VisionEngine::ocr(params);

        if (visionResult.success) {
            RecognitionResult result;
            result.success = visionResult.success;
            result.box.x = visionResult.box.x1;
            result.box.y = visionResult.box.y1;
            result.box.width = visionResult.box.width();
            result.box.height = visionResult.box.height();
            result.score = visionResult.score;
            result.text = visionResult.text;

            // 如果设置了inverse，则反转结果
            if (m_inverse) {
                result.success = !result.success;
            }

            results.push_back(result);
        }
    }

    return results;
}

} // namespace Pipeline
