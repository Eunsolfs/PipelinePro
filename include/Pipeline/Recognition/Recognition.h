#pragma once

#include "Pipeline/Common.h"
#include <memory>
#include <string>

namespace Pipeline {

// 前向声明
class RecognitionResult;

// 识别类型枚举
enum class RecognitionType {
    DirectHit,           // 直接命中
    Always,              // 总是成功
    FindColor,           // 找色
    FindMultiColor,      // 多点找色
    FindColorList,       // 找色列表
    FindMultiColorList,  // 多点找色列表
    TemplateMatch,       // 模板匹配
    OCR                  // OCR识别
};

// 识别基类
class PIPELINE_API Recognition {
public:
    // 构造函数
    Recognition(RecognitionType type);
    virtual ~Recognition() = default;

    // 获取识别类型
    RecognitionType getType() const { return m_type; }

    // 设置是否反转结果
    void setInverse(bool inverse) { m_inverse = inverse; }
    bool isInverse() const { return m_inverse; }

    // 纯虚函数，由派生类实现
    virtual RecognitionResult recognize() = 0;
    
    // 解析参数，由派生类实现
    virtual bool parseConfig(const nlohmann::json& config) = 0;
    
    // 工厂方法，根据类型创建识别对象
    static std::unique_ptr<Recognition> create(RecognitionType type, const nlohmann::json& config);

protected:
    RecognitionType m_type;
    bool m_inverse = false;
};

// 将字符串转换为识别类型
RecognitionType stringToRecognitionType(const std::string& typeStr);

// 将识别类型转换为字符串
std::string recognitionTypeToString(RecognitionType type);

} // namespace Pipeline
