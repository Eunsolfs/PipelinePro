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
    
    // 带全局变量的流水线JSON字符串
    const std::string pipelineJson = R"({
        "var_global": ["%iglobalCounter=0", "%sglobalMessage=全局消息"],
        "Start": {
            "var": ["%ilocalCounter=0", "%slocalMessage=局部消息"],
            "next": ["IncrementGlobal"]
        },
        "IncrementGlobal": {
            "recognition": "DirectHit",
            "action": "DoNothing",
            "log": {
                "true": "全局计数器: {%iglobalCounter++} [%sglobalMessage] 局部计数器: [%ilocalCounter]"
            },
            "next": ["IncrementLocal"]
        },
        "IncrementLocal": {
            "recognition": "DirectHit",
            "action": "DoNothing",
            "var": ["%ilocalCounter=5"],
            "log": {
                "true": "局部计数器: {%ilocalCounter++} [%slocalMessage] 全局计数器: [%iglobalCounter]"
            },
            "next": ["CheckCounters"]
        },
        "CheckCounters": {
            "recognition": "DirectHit",
            "action": "DoNothing",
            "condition": "%iglobalCounter<3",
            "log": {
                "true": "继续执行: 全局计数器=[%iglobalCounter], 局部计数器=[%ilocalCounter]"
            },
            "next": ["IncrementGlobal"],
            "interrupt": ["End"]
        },
        "End": {
            "recognition": "DirectHit",
            "action": "DoNothing",
            "log": {
                "true": "执行结束: 全局计数器=[%iglobalCounter], 局部计数器=[%ilocalCounter]"
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
