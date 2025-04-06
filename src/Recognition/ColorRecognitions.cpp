#include "Pipeline/Recognition/ColorRecognitions.h"
#include "Pipeline/RecognitionResult.h"
#include <vision/vision.h>
#include <iostream>

namespace Pipeline {

// FindColorRecognition实现
FindColorRecognition::FindColorRecognition() : Recognition(RecognitionType::FindColor) {
}

bool FindColorRecognition::parseConfig(const nlohmann::json& config) {
    // 解析ROI
    if (config.contains("roi")) {
        m_roi = config["roi"].get<std::vector<int>>();
    }

    // 解析ROI偏移
    if (config.contains("roi_offset")) {
        m_roiOffset = config["roi_offset"].get<std::vector<int>>();
    }

    // 解析颜色
    if (config.contains("color")) {
        m_color = config["color"].get<std::string>();
    }

    // 解析相似度
    if (config.contains("similarity")) {
        m_similarity = config["similarity"].get<double>();
    }

    // 解析方向
    if (config.contains("direction")) {
        m_direction = config["direction"].get<int>();
    }

    return true;
}

RecognitionResult FindColorRecognition::recognize() {
    RecognitionResult result;

    // 创建找色参数
    vision::FindColorParams params;

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

    // 设置颜色
    params.color = m_color;

    // 设置相似度
    params.similarity = m_similarity;

    // 设置方向
    params.direction = m_direction;

    // 执行找色
    auto visionResult = vision::VisionEngine::findColor(params);

    // 转换结果
    result.success = visionResult.success;
    result.box.x = visionResult.box.x1;
    result.box.y = visionResult.box.y1;
    result.box.width = visionResult.box.width();
    result.box.height = visionResult.box.height();
    result.score = visionResult.score;

    // 如果设置了inverse，则反转结果
    if (m_inverse) {
        result.success = !result.success;
    }

    return result;
}

// FindMultiColorRecognition实现
FindMultiColorRecognition::FindMultiColorRecognition() : Recognition(RecognitionType::FindMultiColor) {
}

bool FindMultiColorRecognition::parseConfig(const nlohmann::json& config) {
    // 解析ROI
    if (config.contains("roi")) {
        m_roi = config["roi"].get<std::vector<int>>();
    }

    // 解析ROI偏移
    if (config.contains("roi_offset")) {
        m_roiOffset = config["roi_offset"].get<std::vector<int>>();
    }

    // 解析第一个颜色
    if (config.contains("first_color")) {
        m_firstColor = config["first_color"].get<std::string>();
    }

    // 解析偏移颜色
    if (config.contains("offset_color")) {
        m_offsetColor = config["offset_color"].get<std::string>();
    }

    // 解析相似度
    if (config.contains("similarity")) {
        m_similarity = config["similarity"].get<double>();
    }

    // 解析方向
    if (config.contains("direction")) {
        m_direction = config["direction"].get<int>();
    }

    return true;
}

RecognitionResult FindMultiColorRecognition::recognize() {
    RecognitionResult result;

    // 创建多点找色参数
    vision::FindMultiColorParams params;

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

    // 设置第一个颜色
    params.firstColor = m_firstColor;

    // 设置偏移颜色
    params.offsetColor = m_offsetColor;

    // 设置相似度
    params.similarity = m_similarity;

    // 设置方向
    params.direction = m_direction;

    // 执行多点找色
    auto visionResult = vision::VisionEngine::findMultiColor(params);

    // 转换结果
    result.success = visionResult.success;
    result.box.x = visionResult.box.x1;
    result.box.y = visionResult.box.y1;
    result.box.width = visionResult.box.width();
    result.box.height = visionResult.box.height();
    result.score = visionResult.score;

    // 如果设置了inverse，则反转结果
    if (m_inverse) {
        result.success = !result.success;
    }

    return result;
}

// FindColorListRecognition实现
FindColorListRecognition::FindColorListRecognition() : Recognition(RecognitionType::FindColorList) {
}

bool FindColorListRecognition::parseConfig(const nlohmann::json& config) {
    // 解析ROI
    if (config.contains("roi")) {
        m_roi = config["roi"].get<std::vector<int>>();
    }

    // 解析ROI偏移
    if (config.contains("roi_offset")) {
        m_roiOffset = config["roi_offset"].get<std::vector<int>>();
    }

    // 解析颜色列表
    if (config.contains("color_list")) {
        if (config["color_list"].is_string()) {
            m_colorList.push_back(config["color_list"].get<std::string>());
        } else if (config["color_list"].is_array()) {
            m_colorList = config["color_list"].get<std::vector<std::string>>();
        }
    }

    // 解析相似度
    if (config.contains("similarity")) {
        m_similarity = config["similarity"].get<double>();
    }

    // 解析方向
    if (config.contains("direction")) {
        m_direction = config["direction"].get<int>();
    }

    return true;
}

RecognitionResult FindColorListRecognition::recognize() {
    RecognitionResult result;

    // 如果颜色列表为空，直接返回失败
    if (m_colorList.empty()) {
        result.success = false;

        // 如果设置了inverse，则反转结果
        if (m_inverse) {
            result.success = !result.success;
        }

        return result;
    }

    // 遍历颜色列表，逐个尝试找色
    for (const auto& color : m_colorList) {
        // 创建找色参数
        vision::FindColorParams params;

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

        // 设置颜色
        params.color = color;

        // 设置相似度
        params.similarity = m_similarity;

        // 设置方向
        params.direction = m_direction;

        // 执行找色
        auto visionResult = vision::VisionEngine::findColor(params);

        // 如果找到了颜色，返回结果
        if (visionResult.success) {
            result.success = true;
            result.box.x = visionResult.box.x1;
            result.box.y = visionResult.box.y1;
            result.box.width = visionResult.box.width();
            result.box.height = visionResult.box.height();
            result.score = visionResult.score;

            // 如果设置了inverse，则反转结果
            if (m_inverse) {
                result.success = !result.success;
            }

            return result;
        }
    }

    // 如果所有颜色都没找到，返回失败
    result.success = false;

    // 如果设置了inverse，则反转结果
    if (m_inverse) {
        result.success = !result.success;
    }

    return result;
}

// FindMultiColorListRecognition实现
FindMultiColorListRecognition::FindMultiColorListRecognition() : Recognition(RecognitionType::FindMultiColorList) {
}

bool FindMultiColorListRecognition::parseConfig(const nlohmann::json& config) {
    // 解析ROI
    if (config.contains("roi")) {
        m_roi = config["roi"].get<std::vector<int>>();
    }

    // 解析ROI偏移
    if (config.contains("roi_offset")) {
        m_roiOffset = config["roi_offset"].get<std::vector<int>>();
    }

    // 解析多点找色列表
    if (config.contains("multi_color_list") && config["multi_color_list"].is_array()) {
        for (const auto& item : config["multi_color_list"]) {
            if (item.is_array() && item.size() >= 2) {
                std::string firstColor = item[0];
                std::string offsetColor = item[1];
                m_multiColorList.push_back(std::make_pair(firstColor, offsetColor));
            }
        }
    }

    // 解析相似度
    if (config.contains("similarity")) {
        m_similarity = config["similarity"].get<double>();
    }

    // 解析方向
    if (config.contains("direction")) {
        m_direction = config["direction"].get<int>();
    }

    return true;
}

RecognitionResult FindMultiColorListRecognition::recognize() {
    RecognitionResult result;

    // 如果多点找色列表为空，直接返回失败
    if (m_multiColorList.empty()) {
        result.success = false;

        // 如果设置了inverse，则反转结果
        if (m_inverse) {
            result.success = !result.success;
        }

        return result;
    }

    // 遍历多点找色列表，逐个尝试多点找色
    for (const auto& [firstColor, offsetColor] : m_multiColorList) {
        // 创建多点找色参数
        vision::FindMultiColorParams params;

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

        // 设置第一个颜色
        params.firstColor = firstColor;

        // 设置偏移颜色
        params.offsetColor = offsetColor;

        // 设置相似度
        params.similarity = m_similarity;

        // 设置方向
        params.direction = m_direction;

        // 执行多点找色
        auto visionResult = vision::VisionEngine::findMultiColor(params);

        // 如果找到了多点找色，返回结果
        if (visionResult.success) {
            result.success = true;
            result.box.x = visionResult.box.x1;
            result.box.y = visionResult.box.y1;
            result.box.width = visionResult.box.width();
            result.box.height = visionResult.box.height();
            result.score = visionResult.score;

            // 如果设置了inverse，则反转结果
            if (m_inverse) {
                result.success = !result.success;
            }

            return result;
        }
    }

    // 如果所有多点找色都没找到，返回失败
    result.success = false;

    // 如果设置了inverse，则反转结果
    if (m_inverse) {
        result.success = !result.success;
    }

    return result;
}

} // namespace Pipeline
