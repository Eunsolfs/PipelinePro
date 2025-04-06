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
    
    // 使用变量表达式的JSON字符串
    const std::string pipelineJson = R"({
        "var_global": ["%ix=100", "%iy=200", "%iwidth=300", "%iheight=400"],
        "Start": {
            "next": ["ClickWithVariables"]
        },
        "ClickWithVariables": {
            "recognition": {
                "type": "DirectHit"
            },
            "action": {
                "type": "Click",
                "target": "[%ix],[%iy]"
            },
            "log": {
                "true": "Clicking at coordinates from variables: [%ix],[%iy]"
            },
            "next": ["SwipeWithVariables"]
        },
        "SwipeWithVariables": {
            "recognition": {
                "type": "DirectHit"
            },
            "action": {
                "type": "Swipe",
                "begin": "[%ix],[%iy]",
                "end": "[%iwidth],[%iheight]",
                "duration": 500
            },
            "log": {
                "true": "Swiping from [%ix],[%iy] to [%iwidth],[%iheight]"
            },
            "next": ["ClickWithCalculation"]
        },
        "ClickWithCalculation": {
            "recognition": {
                "type": "DirectHit"
            },
            "action": {
                "type": "Click",
                "target": "{%ix+50},{%iy+50}"
            },
            "log": {
                "true": "Clicking at calculated coordinates: [%ix+50],[%iy+50]"
            },
            "next": ["End"]
        },
        "End": {
            "recognition": {
                "type": "DirectHit"
            },
            "action": {
                "type": "DoNothing"
            },
            "log": {
                "true": "Action with variables test completed"
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
