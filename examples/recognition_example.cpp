#include <PipelineLib.h>
#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>

int main() {
    // 初始化库
    if (!PipelineInit()) {
        std::cerr << "Failed to initialize PipelineLib" << std::endl;
        return 1;
    }
    
    // 初始化vision库
    if (!vision::VisionEngine::initialize()) {
        std::cerr << "Failed to initialize vision engine" << std::endl;
        PipelineCleanup();
        return 1;
    }
    
    // 加载测试图像
    cv::Mat image = cv::imread("test_image.jpg");
    if (image.empty()) {
        std::cerr << "Failed to load test image" << std::endl;
        vision::VisionEngine::cleanup();
        PipelineCleanup();
        return 1;
    }
    
    // 设置截图
    vision::VisionEngine::setScreenshot(image);
    
    // 创建执行器
    Pipeline::PipelineExecutor* executor = PipelineCreateExecutor();
    if (!executor) {
        std::cerr << "Failed to create executor" << std::endl;
        vision::VisionEngine::cleanup();
        PipelineCleanup();
        return 1;
    }
    
    // 使用各种Recognition的JSON字符串
    const std::string pipelineJson = R"({
        "Start": {
            "next": ["FindColor"]
        },
        "FindColor": {
            "recognition": "FindColor",
            "action": "Click",
            "roi": [0, 0, 1920, 1080],
            "color": "FF0000-101010",
            "similarity": 0.9,
            "direction": 0,
            "next": ["FindMultiColor"]
        },
        "FindMultiColor": {
            "recognition": "FindMultiColor",
            "action": "Click",
            "roi": [0, 0, 1920, 1080],
            "multi_color": "FF0000-101010|5|5|00FF00-101010,10|10|0000FF-101010",
            "similarity": 0.9,
            "direction": 0,
            "next": ["TemplateMatch"]
        },
        "TemplateMatch": {
            "recognition": "TemplateMatch",
            "action": "Click",
            "roi": [0, 0, 1920, 1080],
            "templates": ["template1.jpg", "template2.jpg"],
            "thresholds": [0.7, 0.8],
            "method": 5,
            "next": ["OCR"]
        },
        "OCR": {
            "recognition": "OCR",
            "action": "Click",
            "roi": [0, 0, 1920, 1080],
            "expected": ["example", "text"],
            "model": "ocr_model",
            "next": ["End"]
        },
        "End": {
            "recognition": "DirectHit",
            "action": "DoNothing",
            "log": {
                "true": "Recognition test completed"
            }
        }
    })";
    
    // 执行流水线
    if (!PipelineExecuteFromString(executor, pipelineJson.c_str(), "Start")) {
        std::cerr << "Failed to execute pipeline" << std::endl;
        PipelineDestroyExecutor(executor);
        vision::VisionEngine::cleanup();
        PipelineCleanup();
        return 1;
    }
    
    // 等待用户输入，以便观察执行过程
    std::cout << "Pipeline is running. Press Enter to stop..." << std::endl;
    std::cin.get();
    
    // 停止执行
    PipelineStop(executor);
    
    // 清理资源
    PipelineDestroyExecutor(executor);
    vision::VisionEngine::cleanup();
    PipelineCleanup();
    
    return 0;
}
