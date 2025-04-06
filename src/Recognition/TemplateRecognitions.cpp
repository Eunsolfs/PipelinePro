#include "Pipeline/Recognition/TemplateRecognitions.h"
#include "Pipeline/RecognitionResult.h"
#include <vision/vision.h>
#include <iostream>

namespace Pipeline {

// TemplateMatchRecognition实现
TemplateMatchRecognition::TemplateMatchRecognition() : Recognition(RecognitionType::TemplateMatch) {
}

bool TemplateMatchRecognition::parseConfig(const nlohmann::json& config) {
    // 解析ROI
    if (config.contains("roi")) {
        m_roi = config["roi"].get<std::vector<int>>();
    }

    // 解析ROI偏移
    if (config.contains("roi_offset")) {
        m_roiOffset = config["roi_offset"].get<std::vector<int>>();
    }

    // 解析模板
    if (config.contains("template")) {
        if (config["template"].is_string()) {
            m_templates.push_back(config["template"].get<std::string>());
        } else if (config["template"].is_array()) {
            m_templates = config["template"].get<std::vector<std::string>>();
        }
    }

    // 解析阈值
    if (config.contains("threshold")) {
        if (config["threshold"].is_number()) {
            m_thresholds.push_back(config["threshold"].get<double>());
        } else if (config["threshold"].is_array()) {
            m_thresholds = config["threshold"].get<std::vector<double>>();
        }
    }

    // 解析方法
    if (config.contains("method")) {
        m_method = config["method"].get<int>();
    }
    
    return true;
}

RecognitionResult TemplateMatchRecognition::recognize() {
    RecognitionResult result;
    
    // 如果模板列表为空，直接返回失败
    if (m_templates.empty()) {
        result.success = false;
        
        // 如果设置了inverse，则反转结果
        if (m_inverse) {
            result.success = !result.success;
        }
        
        return result;
    }
    
    // 创建模板匹配参数
    vision::TemplateMatchParams params;
    
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
    
    // 设置模板
    params.templatePaths = m_templates;
    
    // 设置阈值
    if (m_thresholds.empty()) {
        // 如果没有设置阈值，使用默认值
        params.thresholds.push_back(0.8);
    } else {
        params.thresholds = m_thresholds;
    }
    
    // 设置方法
    params.method = m_method;
    
    // 执行模板匹配
    auto visionResult = vision::VisionEngine::templateMatch(params);
    
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

} // namespace Pipeline
