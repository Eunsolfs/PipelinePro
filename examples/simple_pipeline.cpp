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
    
    // 简单的流水线JSON字符串
    const std::string pipelineJson = R"({
        "StartFruit": {
            "next": [
                "Apple",
                "Orange",
                "Banana"
            ]
        },
        "Apple": {
            "recognition": "DirectHit",
            "action": "Click"
        },
        "Orange": {
            "recognition": "DirectHit",
            "action": "Click",
            "next": [
                "Cat",
                "Dog"
            ]
        },
        "Banana": {
            "recognition": "DirectHit",
            "action": "Click"
        },
        "Cat": {
            "recognition": "DirectHit",
            "action": "Swipe",
            "next": []
        },
        "Dog": {
            "recognition": "DirectHit",
            "action": "Click",
            "next": []
        }
    })";
    
    // 执行流水线
    if (!PipelineExecuteFromString(executor, pipelineJson.c_str(), "StartFruit")) {
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
