#include <PipelineLib.h>
#include <iostream>
#include <string>

// 任务停止回调函数
void onTaskStop(const char* nodeName, const char* reason) {
    std::cout << "Task stopped at node: " << nodeName << std::endl;
    std::cout << "Reason: " << reason << std::endl;
}

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
    
    // 设置任务停止回调
    PipelineSetTaskStopCallback(executor, onTaskStop);
    
    // 带StopTask动作的流水线JSON字符串
    const std::string pipelineJson = R"({
        "var_global": ["%icounter=0", "%smessage=开始执行"],
        "Start": {
            "next": ["Counter"]
        },
        "Counter": {
            "recognition": "DirectHit",
            "action": "DoNothing",
            "log": {
                "true": "计数器: {%icounter++} [%smessage]"
            },
            "next": ["CheckCounter"]
        },
        "CheckCounter": {
            "recognition": "DirectHit",
            "action": "DoNothing",
            "condition": "%icounter<3",
            "log": {
                "true": "继续执行: 计数器=[%icounter]",
                "false": "计数器达到上限: [%icounter]"
            },
            "next": ["Counter"],
            "interrupt": ["StopTaskNode"]
        },
        "StopTaskNode": {
            "recognition": "DirectHit",
            "action": "StopTask",
            "log": {
                "true": "执行StopTask动作，主动停止任务"
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
