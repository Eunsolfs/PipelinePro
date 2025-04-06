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
    
    // 带变量的流水线JSON字符串
    const std::string pipelineJson = R"({
        "Start": {
            "var": ["%inumOfBattle=0", "%iMaxNum=5", "%iIntervalNum=2", "%sinformation=任务开始执行"],
            "next": ["Battle"]
        },
        "Battle": {
            "recognition": "DirectHit",
            "action": "Click",
            "condition": "%inumOfBattle<3",
            "log": {
                "true": "执行成功 {%inumOfBattle++} [%sinformation] 当前战斗次数: [%inumOfBattle]",
                "false": "执行失败 {%inumOfBattle--} [%sinformation] 当前战斗次数: [%inumOfBattle] 停止任务"
            },
            "next": ["CheckBattleCount"]
        },
        "CheckBattleCount": {
            "recognition": "DirectHit",
            "action": "DoNothing",
            "condition": "%inumOfBattle<3",
            "log": {
                "true": "继续战斗 {%inumOfBattle=%iMaxNum-%iIntervalNum} 当前战斗次数: [%inumOfBattle]",
                "false": "战斗次数已达上限 当前战斗次数: [%inumOfBattle]"
            },
            "next": ["Battle"],
            "interrupt": ["End"]
        },
        "End": {
            "recognition": "DirectHit",
            "action": "DoNothing",
            "log": {
                "true": "任务结束 总共战斗次数: [%inumOfBattle]"
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
