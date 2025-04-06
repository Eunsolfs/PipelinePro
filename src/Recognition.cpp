#include "Pipeline/Recognition.h"
#include <nlohmann/json.hpp>

namespace Pipeline {

// 创建识别对象的工厂方法
std::unique_ptr<Recognition> Recognition::create(RecognitionType type, const nlohmann::json& config) {
    std::unique_ptr<Recognition> recognition;

    switch (type) {
        case RecognitionType::DirectHit:
            recognition = std::make_unique<DirectHitRecognition>();
            break;
        case RecognitionType::TemplateMatch:
            recognition = std::make_unique<TemplateMatchRecognition>();
            break;
        case RecognitionType::OCR:
            recognition = std::make_unique<OCRRecognition>();
            break;
        case RecognitionType::FindColor:
            recognition = std::make_unique<FindColorRecognition>();
            break;
        case RecognitionType::FindColorList:
            recognition = std::make_unique<FindColorListRecognition>();
            break;
        case RecognitionType::FindMultiColor:
            recognition = std::make_unique<FindMultiColorRecognition>();
            break;
        case RecognitionType::FindMultiColorList:
            recognition = std::make_unique<FindMultiColorListRecognition>();
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
    // DirectHit不需要额外的参数
    return true;
}

// TemplateMatchRecognition实现
TemplateMatchRecognition::TemplateMatchRecognition()
    : Recognition(RecognitionType::TemplateMatch) {
}

bool TemplateMatchRecognition::parseConfig(const nlohmann::json& config) {
    // 解析ROI
    if (config.contains("roi")) {
        if (config["roi"].is_array()) {
            m_roi = config["roi"].get<std::vector<int>>();
        } else if (config["roi"].is_string()) {
            // 这里应该处理引用其他节点ROI的情况
            // 实际实现中需要保存引用的节点名称，并在执行时获取该节点的ROI
        }
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
    } else {
        // 默认阈值
        m_thresholds.resize(m_templates.size(), 0.7);
    }

    // 解析排序方式
    if (config.contains("order_by")) {
        m_orderBy = config["order_by"].get<std::string>();
    }

    // 解析匹配方法
    if (config.contains("method")) {
        m_method = config["method"].get<int>();
    }

    return true;
}

RecognitionResult TemplateMatchRecognition::recognize() {
    // 使用vision库进行模板匹配

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

    // 设置模板路径
    params.templatePaths = m_templates;

    // 设置阈值
    params.thresholds = m_thresholds;

    // 设置排序方式
    params.orderBy = m_orderBy;

    // 设置匹配方法
    params.method = m_method;

    // 执行模板匹配
    auto visionResult = vision::VisionEngine::templateMatch(params);

    // 转换结果
    RecognitionResult result;
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

// OCRRecognition实现
OCRRecognition::OCRRecognition()
    : Recognition(RecognitionType::OCR) {
}

bool OCRRecognition::parseConfig(const nlohmann::json& config) {
    // 解析ROI
    if (config.contains("roi")) {
        if (config["roi"].is_array()) {
            m_roi = config["roi"].get<std::vector<int>>();
        } else if (config["roi"].is_string()) {
            // 处理引用其他节点ROI的情况
        }
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
    if (config.contains("replace")) {
        if (config["replace"].is_array() && config["replace"].size() == 2 &&
            config["replace"][0].is_string() && config["replace"][1].is_string()) {
            m_replace.push_back({
                config["replace"][0].get<std::string>(),
                config["replace"][1].get<std::string>()
            });
        } else if (config["replace"].is_array()) {
            for (const auto& item : config["replace"]) {
                if (item.is_array() && item.size() == 2 &&
                    item[0].is_string() && item[1].is_string()) {
                    m_replace.push_back({
                        item[0].get<std::string>(),
                        item[1].get<std::string>()
                    });
                }
            }
        }
    }

    // 解析排序方式
    if (config.contains("order_by")) {
        m_orderBy = config["order_by"].get<std::string>();
    }

    // 解析索引
    if (config.contains("index")) {
        m_index = config["index"].get<int>();
    }

    // 解析是否仅识别
    if (config.contains("only_rec")) {
        m_onlyRec = config["only_rec"].get<bool>();
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

// FindColorRecognition实现
FindColorRecognition::FindColorRecognition()
    : Recognition(RecognitionType::FindColor) {
}

bool FindColorRecognition::parseConfig(const nlohmann::json& config) {
    // 解析ROI
    if (config.contains("roi")) {
        if (config["roi"].is_array()) {
            m_roi = config["roi"].get<std::vector<int>>();
        } else if (config["roi"].is_string()) {
            // 处理引用其他节点ROI的情况
        }
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

    // 解析查找方向
    if (config.contains("direction")) {
        m_direction = config["direction"].get<int>();
    }

    return true;
}

RecognitionResult FindColorRecognition::recognize() {
    // 使用vision库进行找色

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

    // 设置查找方向
    params.direction = m_direction;

    // 执行找色
    auto visionResult = vision::VisionEngine::findColor(params);

    // 转换结果
    RecognitionResult result;
    result.success = visionResult.success;
    result.box.x1 = visionResult.box.x1;
    result.box.y1 = visionResult.box.y1;
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
FindColorListRecognition::FindColorListRecognition()
    : Recognition(RecognitionType::FindColorList) {
}

bool FindColorListRecognition::parseConfig(const nlohmann::json& config) {
    // 解析ROI
    if (config.contains("roi")) {
        if (config["roi"].is_array()) {
            m_roi = config["roi"].get<std::vector<int>>();
        } else if (config["roi"].is_string()) {
            // 处理引用其他节点ROI的情况
        }
    }

    // 解析ROI偏移
    if (config.contains("roi_offset")) {
        m_roiOffset = config["roi_offset"].get<std::vector<int>>();
    }

    // 解析颜色列表
    if (config.contains("color_list")) {
        m_colorList = config["color_list"].get<std::vector<std::string>>();
    }

    // 解析相似度
    if (config.contains("similarity")) {
        m_similarity = config["similarity"].get<double>();
    }

    // 解析查找方向
    if (config.contains("direction")) {
        m_direction = config["direction"].get<int>();
    }

    return true;
}

RecognitionResult FindColorListRecognition::recognize() {
    // 实现找色列表算法，对每个颜色进行找色
    RecognitionResult result;
    result.success = false;

    // 如果颜色列表为空，直接返回失败
    if (m_colorList.empty()) {
        if (m_inverse) {
            result.success = !result.success;
        }
        return result;
    }

    // 对每个颜色进行找色
    for (const auto& color : m_colorList) {
        // 创建一个FindColorRecognition对象
        auto findColor = std::make_unique<FindColorRecognition>();

        // 设置参数
        nlohmann::json config;
        config["roi"] = m_roi;
        config["roi_offset"] = m_roiOffset;
        config["color"] = color;
        config["similarity"] = m_similarity;
        config["direction"] = m_direction;

        // 解析参数
        findColor->parseConfig(config);

        // 执行找色
        auto colorResult = findColor->recognize();

        // 如果找到了颜色，返回成功
        if (colorResult.success) {
            result = colorResult;
            result.success = true;
            break;
        }
    }

    // 如果设置了inverse，则反转结果
    if (m_inverse) {
        result.success = !result.success;
    }

    return result;
}

// FindMultiColorRecognition实现
FindMultiColorRecognition::FindMultiColorRecognition()
    : Recognition(RecognitionType::FindMultiColor) {
}

bool FindMultiColorRecognition::parseConfig(const nlohmann::json& config) {
    // 解析ROI
    if (config.contains("roi")) {
        if (config["roi"].is_array()) {
            m_roi = config["roi"].get<std::vector<int>>();
        } else if (config["roi"].is_string()) {
            // 处理引用其他节点ROI的情况
        }
    }

    // 解析ROI偏移
    if (config.contains("roi_offset")) {
        m_roiOffset = config["roi_offset"].get<std::vector<int>>();
    }

    // 解析多点找色
    if (config.contains("multi_color")) {
        m_multiColor = config["multi_color"].get<std::string>();
    }

    // 解析相似度
    if (config.contains("similarity")) {
        m_similarity = config["similarity"].get<double>();
    }

    // 解析查找方向
    if (config.contains("direction")) {
        m_direction = config["direction"].get<int>();
    }

    return true;
}

RecognitionResult FindMultiColorRecognition::recognize() {
    // 使用vision库进行多点找色

    // 解析多点找色参数
    std::string firstColor;
    std::string offsetColor;

    // 解析m_multiColor字符串，格式为"firstColor|offsetColor"
    size_t pos = m_multiColor.find('|');
    if (pos != std::string::npos) {
        firstColor = m_multiColor.substr(0, pos);
        offsetColor = m_multiColor.substr(pos + 1);
    } else {
        // 格式错误，返回失败
        RecognitionResult result;
        result.success = false;

        if (m_inverse) {
            result.success = !result.success;
        }

        return result;
    }

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

    // 设置第一个颜色和偏移颜色
    params.firstColor = firstColor;
    params.offsetColor = offsetColor;

    // 设置相似度
    params.similarity = m_similarity;

    // 设置查找方向
    params.direction = m_direction;

    // 执行多点找色
    auto visionResult = vision::VisionEngine::findMultiColor(params);

    // 转换结果
    RecognitionResult result;
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

// FindMultiColorListRecognition实现
FindMultiColorListRecognition::FindMultiColorListRecognition()
    : Recognition(RecognitionType::FindMultiColorList) {
}

bool FindMultiColorListRecognition::parseConfig(const nlohmann::json& config) {
    // 解析ROI
    if (config.contains("roi")) {
        if (config["roi"].is_array()) {
            m_roi = config["roi"].get<std::vector<int>>();
        } else if (config["roi"].is_string()) {
            // 处理引用其他节点ROI的情况
        }
    }

    // 解析ROI偏移
    if (config.contains("roi_offset")) {
        m_roiOffset = config["roi_offset"].get<std::vector<int>>();
    }

    // 解析多点找色列表
    if (config.contains("multi_color_list")) {
        m_multiColorList = config["multi_color_list"].get<std::vector<std::string>>();
    }

    // 解析相似度
    if (config.contains("similarity")) {
        m_similarity = config["similarity"].get<double>();
    }

    // 解析查找方向
    if (config.contains("direction")) {
        m_direction = config["direction"].get<int>();
    }

    return true;
}

RecognitionResult FindMultiColorListRecognition::recognize() {
    // 实现多点找色列表算法，对每个多点找色进行匹配
    RecognitionResult result;
    result.success = false;

    // 如果多点找色列表为空，直接返回失败
    if (m_multiColorList.empty()) {
        if (m_inverse) {
            result.success = !result.success;
        }
        return result;
    }

    // 对每个多点找色进行匹配
    for (const auto& multiColor : m_multiColorList) {
        // 创建一个FindMultiColorRecognition对象
        auto findMultiColor = std::make_unique<FindMultiColorRecognition>();

        // 设置参数
        nlohmann::json config;
        config["roi"] = m_roi;
        config["roi_offset"] = m_roiOffset;
        config["multi_color"] = multiColor;
        config["similarity"] = m_similarity;
        config["direction"] = m_direction;

        // 解析参数
        findMultiColor->parseConfig(config);

        // 执行多点找色
        auto multiColorResult = findMultiColor->recognize();

        // 如果找到了多点找色，返回成功
        if (multiColorResult.success) {
            result = multiColorResult;
            result.success = true;
            break;
        }
    }

    // 如果设置了inverse，则反转结果
    if (m_inverse) {
        result.success = !result.success;
    }

    return result;
}

} // namespace Pipeline
