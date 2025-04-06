#include <gtest/gtest.h>
#include <PipelineLib.h>
#include <string>

// 测试节点的识别和动作执行
TEST(NodeExecutionTest, RecognitionAndAction) {
    // 创建一个简单的流水线，只包含一个节点
    const std::string pipelineJson = R"({
        "TestNode": {
            "recognition": "DirectHit",
            "action": "DoNothing"
        }
    })";
    
    // 创建执行器
    Pipeline::Pipeline pipeline;
    EXPECT_TRUE(pipeline.loadFromString(pipelineJson));
    
    // 获取节点
    auto node = pipeline.getNode("TestNode");
    ASSERT_NE(node, nullptr);
    
    // 测试识别
    auto result = node->executeRecognition();
    EXPECT_TRUE(result.success);
    
    // 测试动作
    EXPECT_TRUE(node->executeAction(result));
}

// 测试节点的启用/禁用
TEST(NodeExecutionTest, EnableDisable) {
    // 创建一个简单的流水线，包含一个禁用的节点
    const std::string pipelineJson = R"({
        "EnabledNode": {
            "recognition": "DirectHit",
            "action": "DoNothing",
            "enabled": true
        },
        "DisabledNode": {
            "recognition": "DirectHit",
            "action": "DoNothing",
            "enabled": false
        }
    })";
    
    // 创建执行器
    Pipeline::Pipeline pipeline;
    EXPECT_TRUE(pipeline.loadFromString(pipelineJson));
    
    // 获取节点
    auto enabledNode = pipeline.getNode("EnabledNode");
    auto disabledNode = pipeline.getNode("DisabledNode");
    ASSERT_NE(enabledNode, nullptr);
    ASSERT_NE(disabledNode, nullptr);
    
    // 测试启用/禁用状态
    EXPECT_TRUE(enabledNode->isEnabled());
    EXPECT_FALSE(disabledNode->isEnabled());
    
    // 测试识别
    auto enabledResult = enabledNode->executeRecognition();
    EXPECT_TRUE(enabledResult.success);
    
    auto disabledResult = disabledNode->executeRecognition();
    EXPECT_FALSE(disabledResult.success);
}

// 测试节点的延迟
TEST(NodeExecutionTest, Delays) {
    // 创建一个简单的流水线，包含带延迟的节点
    const std::string pipelineJson = R"({
        "DelayNode": {
            "recognition": "DirectHit",
            "action": "DoNothing",
            "pre_delay": 100,
            "post_delay": 100
        }
    })";
    
    // 创建执行器
    Pipeline::Pipeline pipeline;
    EXPECT_TRUE(pipeline.loadFromString(pipelineJson));
    
    // 获取节点
    auto node = pipeline.getNode("DelayNode");
    ASSERT_NE(node, nullptr);
    
    // 测试延迟
    EXPECT_EQ(node->getPreDelay(), 100);
    EXPECT_EQ(node->getPostDelay(), 100);
    
    // 测试识别和动作执行（包含延迟）
    auto start = std::chrono::steady_clock::now();
    
    auto result = node->executeRecognition();
    EXPECT_TRUE(result.success);
    
    EXPECT_TRUE(node->executeAction(result));
    
    auto end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    
    // 验证执行时间至少包含了延迟时间
    EXPECT_GE(duration, 200);
}
