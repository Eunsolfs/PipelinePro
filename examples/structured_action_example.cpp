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
    
    // 使用新的结构化action格式的JSON字符串
    const std::string pipelineJson = R"({
        "Start": {
            "next": ["Click"]
        },
        "Click": {
            "recognition": "DirectHit",
            "action": {
                "type": "Click",
                "target": [100, 200],
                "target_offset": [10, 10, 0, 0]
            },
            "next": ["Swipe"]
        },
        "Swipe": {
            "recognition": "DirectHit",
            "action": {
                "type": "Swipe",
                "begin": [100, 200],
                "end": [300, 400],
                "duration": 500
            },
            "next": ["Key"]
        },
        "Key": {
            "recognition": "DirectHit",
            "action": {
                "type": "Key",
                "key": [13, 27]
            },
            "next": ["Text"]
        },
        "Text": {
            "recognition": "DirectHit",
            "action": {
                "type": "Text",
                "input_text": "Hello, World!"
            },
            "next": ["StartApp"]
        },
        "StartApp": {
            "recognition": "DirectHit",
            "action": {
                "type": "StartApp",
                "package": "com.example.app"
            },
            "next": ["StopApp"]
        },
        "StopApp": {
            "recognition": "DirectHit",
            "action": {
                "type": "StopApp",
                "package": "com.example.app"
            },
            "next": ["Command"]
        },
        "Command": {
            "recognition": "DirectHit",
            "action": {
                "type": "Command",
                "exec": "echo",
                "args": ["Hello", "World"],
                "detach": false
            },
            "next": ["StopTask"]
        },
        "StopTask": {
            "recognition": "DirectHit",
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
