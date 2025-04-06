#include <PipelineLib.h>
#include <iostream>
#include <string>
#include <vector>

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
    
    // 使用批量OCR功能的JSON字符串
    const std::string pipelineJson = R"({
        "Start": {
            "next": ["OCRBatch"]
        },
        "OCRBatch": {
            "recognition": "OCR",
            "action": {
                "type": "DoNothing"
            },
            "roi": [0, 0, 1920, 1080],
            "expected": [],
            "replace": [["\\s+", " "]],
            "orderBy": "vertical",
            "index": 0,
            "onlyRec": false,
            "model": "",
            "log": {
                "true": "OCR识别成功: [%text]"
            },
            "next": ["ProcessOCRResults"]
        },
        "ProcessOCRResults": {
            "recognition": "DirectHit",
            "action": {
                "type": "Command",
                "exec": "echo",
                "args": ["OCR识别完成"],
                "detach": false
            },
            "log": {
                "true": "处理OCR结果"
            },
            "next": ["End"]
        },
        "End": {
            "recognition": "DirectHit",
            "action": {
                "type": "DoNothing"
            },
            "log": {
                "true": "批量OCR示例完成"
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
