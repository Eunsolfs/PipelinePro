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
    
    // 带condition_process的流水线JSON字符串
    const std::string pipelineJson = R"({
        "var_global": ["%inumOfBattle=0", "%iMaxNum=5", "%iIntervalNum=2", "%iallnumOfBattle=0"],
        "Start": {
            "var": ["%slocalMessage=开始执行"],
            "next": ["Battle"]
        },
        "Battle": {
            "recognition": "DirectHit",
            "action": "Click",
            "condition": "%inumOfBattle<3",
            "condition_process": {
                "true": {
                    "override_next": ["CheckBattleCount"],
                    "override_interrupt": ["CheckNum"],
                    "var_operation": "{%inumOfBattle++} {%iallnumOfBattle++}",
                    "condition_log": "condition_log => 第 [%inumOfBattle] / [%iallnumOfBattle] 次"
                },
                "false": {
                    "override_next": ["End"],
                    "condition_log": "任务失败，停止任务"
                }
            },
            "log": {
                "true": "执行成功 当前战斗次数: [%inumOfBattle]"
            },
            "next": ["OriginalNext"],
            "interrupt": ["OriginalInterrupt"]
        },
        "OriginalNext": {
            "recognition": "DirectHit",
            "action": "DoNothing",
            "log": {
                "true": "这是原始的next节点，不应该被执行到"
            }
        },
        "OriginalInterrupt": {
            "recognition": "DirectHit",
            "action": "DoNothing",
            "log": {
                "true": "这是原始的interrupt节点，不应该被执行到"
            }
        },
        "CheckBattleCount": {
            "recognition": "DirectHit",
            "action": "DoNothing",
            "condition": "%inumOfBattle<3",
            "condition_process": {
                "true": {
                    "var_operation": "{%inumOfBattle=%iMaxNum-%iIntervalNum}",
                    "condition_log": "继续战斗，设置战斗次数为: [%inumOfBattle]"
                },
                "false": {
                    "condition_log": "战斗次数已达上限: [%inumOfBattle]"
                }
            },
            "next": ["Battle"],
            "interrupt": ["End"]
        },
        "CheckNum": {
            "recognition": "DirectHit",
            "action": "DoNothing",
            "log": {
                "true": "这是通过override_interrupt设置的节点"
            },
            "next": ["Battle"]
        },
        "End": {
            "recognition": "DirectHit",
            "action": "DoNothing",
            "log": {
                "true": "任务结束 总共战斗次数: [%inumOfBattle], 总计: [%iallnumOfBattle]"
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
