#include <PipelineLib.h>
#include <iostream>
#include <string>

int main() {
    // 初始化库
    if (!PipelineInit()) {
        std::cerr << "Failed to initialize PipelineLib" << std::endl;
        return 1;
    }
    
    // 创建执行器
    Pipeline::PipelineExecutor* executor = PipelineCreateExecutor();
    if (!executor) {
        std::cerr << "Failed to create executor" << std::endl;
        PipelineCleanup();
        return 1;
    }
    
    // 使用完全结构化的JSON字符串（recognition和action都使用对象格式）
    const std::string pipelineJson = R"({
        "Start": {
            "next": ["FindColor"]
        },
        "FindColor": {
            "recognition": {
                "type": "FindColor",
                "roi": [0, 0, 1920, 1080],
                "color": "FF0000-101010",
                "similarity": 0.9,
                "direction": 0
            },
            "action": {
                "type": "Click",
                "target": [100, 200],
                "target_offset": [10, 10, 0, 0]
            },
            "next": ["FindMultiColor"]
        },
        "FindMultiColor": {
            "recognition": {
                "type": "FindMultiColor",
                "roi": [0, 0, 1920, 1080],
                "multi_color": "FF0000-101010|5|5|00FF00-101010,10|10|0000FF-101010",
                "similarity": 0.9,
                "direction": 0
            },
            "action": {
                "type": "Click",
                "target": true
            },
            "next": ["TemplateMatch"]
        },
        "TemplateMatch": {
            "recognition": {
                "type": "TemplateMatch",
                "roi": [0, 0, 1920, 1080],
                "template": ["template1.jpg", "template2.jpg"],
                "threshold": [0.7, 0.8],
                "method": 5
            },
            "action": {
                "type": "Swipe",
                "begin": [100, 200],
                "end": [300, 400],
                "duration": 500
            },
            "next": ["OCR"]
        },
        "OCR": {
            "recognition": {
                "type": "OCR",
                "roi": [0, 0, 1920, 1080],
                "expected": ["example", "text"],
                "model": "ocr_model"
            },
            "action": {
                "type": "Text",
                "input_text": "Hello, World!"
            },
            "next": ["End"]
        },
        "End": {
            "recognition": {
                "type": "DirectHit"
            },
            "action": {
                "type": "StopTask"
            }
        }
    })";
    
    // 执行流水线
    if (!PipelineExecuteFromString(executor, pipelineJson.c_str(), "Start")) {
        std::cerr << "Failed to execute pipeline" << std::endl;
        PipelineDestroyExecutor(executor);
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
    PipelineCleanup();
    
    return 0;
}
