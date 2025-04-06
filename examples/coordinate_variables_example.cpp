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
    
    // 使用坐标类型变量的JSON字符串
    const std::string pipelineJson = R"({
        "var_global": ["%pStartPoint=100,200", "%pEndPoint=300,400", "%rArea=50,50,250,250"],
        "Start": {
            "next": ["ClickWithPointVar"]
        },
        "ClickWithPointVar": {
            "recognition": {
                "type": "DirectHit"
            },
            "action": {
                "type": "Click",
                "target": "%pStartPoint"
            },
            "log": {
                "true": "Clicking at point variable: [%pStartPoint]"
            },
            "next": ["SwipeWithPointVars"]
        },
        "SwipeWithPointVars": {
            "recognition": {
                "type": "DirectHit"
            },
            "action": {
                "type": "Swipe",
                "begin": "%pStartPoint",
                "end": "%pEndPoint",
                "duration": 500
            },
            "log": {
                "true": "Swiping from [%pStartPoint] to [%pEndPoint]"
            },
            "next": ["ClickInRect"]
        },
        "ClickInRect": {
            "recognition": {
                "type": "DirectHit"
            },
            "action": {
                "type": "Click",
                "target": "%rArea"
            },
            "log": {
                "true": "Clicking in rectangle area: [%rArea]"
            },
            "next": ["SaveCoordinates"]
        },
        "SaveCoordinates": {
            "recognition": {
                "type": "DirectHit"
            },
            "action": {
                "type": "Swipe",
                "begin": [150, 150],
                "end": [350, 350],
                "duration": 300
            },
            "log": {
                "true": "Swipe completed. Last swipe begin: [%pLastSwipeBegin], end: [%pLastSwipeEnd], area: [%rLastSwipeArea]"
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
                "true": "Coordinate variables test completed"
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
