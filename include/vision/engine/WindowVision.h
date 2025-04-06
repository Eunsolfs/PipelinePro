#pragma once

#include "../core/Types.h"
#include "../core/Params.h"
#include "../engine/Vision.h"
#include <opencv2/opencv.hpp>
#include <map>
#include <chrono>
#include <shared_mutex>
#include <Windows.h>

namespace vision {

// WindowVision类 - 窗口图像管理和视觉处理
class VISION_API WindowVision {
public:
    // 构造函数
    WindowVision(int cacheTimeoutMs = 100);
    
    // 析构函数
    ~WindowVision();
    
    // 更新窗口图像
    void updateWindowImage(HWND hwnd, const unsigned char* data, int width, int height, int channels);
    
    // 获取窗口对应的Vision对象
    VisionHandle getVisionObject(HWND hwnd);
    
    // 执行找色操作
    bool findColor(HWND hwnd, int x1, int y1, int x2, int y2, const char* color, double sim, int dir, int* outX, int* outY);
    
    // 执行多点找色操作
    bool findMultiColor(HWND hwnd, int x1, int y1, int x2, int y2, const char* firstColor, const char* offsetColor, double sim, int dir, int* outX, int* outY);
    
    // 执行模板匹配操作
    bool templateMatch(HWND hwnd, int x1, int y1, int x2, int y2, const char* templatePath, double threshold, int method, int* outX, int* outY, double* outScore);
    
    // 执行OCR操作
    bool ocr(HWND hwnd, int x1, int y1, int x2, int y2, const char* expected, char* outText, int outTextSize);
    
    // 执行批量OCR操作
    int ocrBatch(HWND hwnd, int x1, int y1, int x2, int y2, const char* expected, char* outResults, int outResultsSize);
    
    // 获取窗口图像的ROI区域（用于调试或其他目的）
    cv::Mat getWindowROI(HWND hwnd, int x1, int y1, int x2, int y2);

private:
    // 窗口图像缓存
    std::map<HWND, cv::Mat> m_windowImages;
    std::map<HWND, std::chrono::steady_clock::time_point> m_lastUpdateTime;
    
    // 窗口对应的Vision对象
    std::map<HWND, VisionHandle> m_visionObjects;
    
    // 互斥锁
    std::shared_mutex m_mutex;
    
    // 缓存超时时间（毫秒）
    int m_cacheTimeoutMs;
};

// WindowVision句柄类型
typedef WindowVision* WindowVisionHandle;

} // namespace vision
