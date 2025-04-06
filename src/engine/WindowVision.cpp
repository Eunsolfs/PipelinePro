#include "vision/engine/WindowVision.h"

namespace vision {

// 构造函数
WindowVision::WindowVision(int cacheTimeoutMs) : m_cacheTimeoutMs(cacheTimeoutMs) {
}

// 析构函数
WindowVision::~WindowVision() {
    // 清理所有Vision对象
    std::unique_lock<std::shared_mutex> lock(m_mutex);
    for (auto& pair : m_visionObjects) {
        if (pair.second) {
            VisionDestroy(pair.second);
        }
    }
    m_visionObjects.clear();
    m_windowImages.clear();
}

// 更新窗口图像
void WindowVision::updateWindowImage(HWND hwnd, const unsigned char* data, int width, int height, int channels) {
    std::unique_lock<std::shared_mutex> lock(m_mutex);
    
    // 更新图像缓存
    cv::Mat image(height, width, channels == 3 ? CV_8UC3 : CV_8UC4, (void*)data);
    m_windowImages[hwnd] = image.clone();
    m_lastUpdateTime[hwnd] = std::chrono::steady_clock::now();
    
    // 确保该窗口有对应的Vision对象
    if (m_visionObjects.find(hwnd) == m_visionObjects.end()) {
        m_visionObjects[hwnd] = VisionCreate();
    }
    
    // 更新Vision对象的截图
    VisionSetScreenshot(m_visionObjects[hwnd], data, width, height, channels);
}

// 获取窗口对应的Vision对象
VisionHandle WindowVision::getVisionObject(HWND hwnd) {
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    
    auto it = m_visionObjects.find(hwnd);
    if (it == m_visionObjects.end()) {
        return nullptr;
    }
    
    // 检查缓存是否过期
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
        now - m_lastUpdateTime[hwnd]).count();
    if (elapsed > m_cacheTimeoutMs) {
        return nullptr; // 缓存过期
    }
    
    return it->second;
}

// 执行找色操作
bool WindowVision::findColor(HWND hwnd, int x1, int y1, int x2, int y2, const char* color, double sim, int dir, int* outX, int* outY) {
    // 获取Vision对象
    VisionHandle vision = getVisionObject(hwnd);
    if (!vision) {
        return false;
    }
    
    // 直接使用Vision对象执行操作
    return VisionFindColor(vision, x1, y1, x2, y2, color, sim, dir, outX, outY);
}

// 执行多点找色操作
bool WindowVision::findMultiColor(HWND hwnd, int x1, int y1, int x2, int y2, const char* firstColor, const char* offsetColor, double sim, int dir, int* outX, int* outY) {
    VisionHandle vision = getVisionObject(hwnd);
    if (!vision) {
        return false;
    }
    
    return VisionFindMultiColor(vision, x1, y1, x2, y2, firstColor, offsetColor, sim, dir, outX, outY);
}

// 执行模板匹配操作
bool WindowVision::templateMatch(HWND hwnd, int x1, int y1, int x2, int y2, const char* templatePath, double threshold, int method, int* outX, int* outY, double* outScore) {
    VisionHandle vision = getVisionObject(hwnd);
    if (!vision) {
        return false;
    }
    
    return VisionTemplateMatch(vision, x1, y1, x2, y2, templatePath, threshold, method, outX, outY, outScore);
}

// 执行OCR操作
bool WindowVision::ocr(HWND hwnd, int x1, int y1, int x2, int y2, const char* expected, char* outText, int outTextSize) {
    VisionHandle vision = getVisionObject(hwnd);
    if (!vision) {
        return false;
    }
    
    return VisionOcr(vision, x1, y1, x2, y2, expected, outText, outTextSize);
}

// 执行批量OCR操作
int WindowVision::ocrBatch(HWND hwnd, int x1, int y1, int x2, int y2, const char* expected, char* outResults, int outResultsSize) {
    VisionHandle vision = getVisionObject(hwnd);
    if (!vision) {
        return 0;
    }
    
    return VisionOcrBatch(vision, x1, y1, x2, y2, expected, outResults, outResultsSize);
}

// 获取窗口图像的ROI区域
cv::Mat WindowVision::getWindowROI(HWND hwnd, int x1, int y1, int x2, int y2) {
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    
    auto it = m_windowImages.find(hwnd);
    if (it == m_windowImages.end()) {
        return cv::Mat();
    }
    
    // 检查缓存是否过期
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
        now - m_lastUpdateTime[hwnd]).count();
    if (elapsed > m_cacheTimeoutMs) {
        return cv::Mat(); // 缓存过期
    }
    
    // 获取ROI
    cv::Rect roi(x1, y1, x2 - x1, y2 - y1);
    if (roi.x < 0 || roi.y < 0 || roi.width <= 0 || roi.height <= 0 ||
        roi.x + roi.width > it->second.cols || roi.y + roi.height > it->second.rows) {
        roi = roi & cv::Rect(0, 0, it->second.cols, it->second.rows);
        if (roi.width <= 0 || roi.height <= 0) {
            return cv::Mat();
        }
    }
    
    return it->second(roi).clone();
}

} // namespace vision
