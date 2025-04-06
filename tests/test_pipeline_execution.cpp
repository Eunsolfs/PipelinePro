#include <gtest/gtest.h>
#include <PipelineLib.h>
#include <string>
#include <thread>
#include <chrono>

// 测试基本的流水线执行
TEST(PipelineExecutionTest, BasicExecution) {
    // 创建一个简单的流水线
    const std::string pipelineJson = R"({
        "Start": {
            "next": ["End"]
        },
        "End": {
            "recognition": "DirectHit",
            "action": "DoNothing"
        }
    })";
    
    // 创建执行器
    Pipeline::PipelineExecutor executor;
    EXPECT_TRUE(executor.executeFromString(pipelineJson, "Start"));
    
    // 等待一段时间，让流水线执行
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    
    // 停止执行
    executor.stop();
}

// 测试带分支的流水线执行
TEST(PipelineExecutionTest, BranchExecution) {
    // 创建一个带分支的流水线
    const std::string pipelineJson = R"({
        "Start": {
            "next": ["Branch1", "Branch2"]
        },
        "Branch1": {
            "recognition": "DirectHit",
            "action": "DoNothing",
            "next": ["End"]
        },
        "Branch2": {
            "recognition": "DirectHit",
            "action": "DoNothing",
            "next": ["End"]
        },
        "End": {
            "recognition": "DirectHit",
            "action": "DoNothing"
        }
    })";
    
    // 创建执行器
    Pipeline::PipelineExecutor executor;
    EXPECT_TRUE(executor.executeFromString(pipelineJson, "Start"));
    
    // 等待一段时间，让流水线执行
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    
    // 停止执行
    executor.stop();
}

// 测试带中断的流水线执行
TEST(PipelineExecutionTest, InterruptExecution) {
    // 创建一个带中断的流水线
    const std::string pipelineJson = R"({
        "Start": {
            "next": ["Normal"],
            "interrupt": ["Interrupt"]
        },
        "Normal": {
            "recognition": "DirectHit",
            "action": "DoNothing",
            "next": ["End"]
        },
        "Interrupt": {
            "recognition": "DirectHit",
            "action": "DoNothing",
            "next": ["End"]
        },
        "End": {
            "recognition": "DirectHit",
            "action": "DoNothing"
        }
    })";
    
    // 创建执行器
    Pipeline::PipelineExecutor executor;
    EXPECT_TRUE(executor.executeFromString(pipelineJson, "Start"));
    
    // 等待一段时间，让流水线执行
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    
    // 停止执行
    executor.stop();
}

// 测试带错误处理的流水线执行
TEST(PipelineExecutionTest, ErrorHandlingExecution) {
    // 创建一个带错误处理的流水线
    const std::string pipelineJson = R"({
        "Start": {
            "next": ["Normal"],
            "on_error": ["Error"]
        },
        "Normal": {
            "recognition": "DirectHit",
            "action": "DoNothing",
            "next": ["End"]
        },
        "Error": {
            "recognition": "DirectHit",
            "action": "DoNothing",
            "next": ["End"]
        },
        "End": {
            "recognition": "DirectHit",
            "action": "DoNothing"
        }
    })";
    
    // 创建执行器
    Pipeline::PipelineExecutor executor;
    EXPECT_TRUE(executor.executeFromString(pipelineJson, "Start"));
    
    // 等待一段时间，让流水线执行
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    
    // 停止执行
    executor.stop();
}
