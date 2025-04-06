#include <gtest/gtest.h>
#include <PipelineLib.h>
#include <string>

TEST(JsonParsingTest, BasicParsing) {
    // 创建执行器
    Pipeline::PipelineExecutor executor;
    
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
    
    // 测试从字符串加载流水线
    EXPECT_TRUE(executor.executeFromString(pipelineJson, "StartFruit"));
}

TEST(JsonParsingTest, InvalidJson) {
    // 创建执行器
    Pipeline::PipelineExecutor executor;
    
    // 无效的JSON字符串
    const std::string invalidJson = R"({
        "StartFruit": {
            "next": [
                "Apple",
                "Orange",
                "Banana"
            ],
        } // 多余的逗号
    })";
    
    // 测试从无效字符串加载流水线
    EXPECT_FALSE(executor.executeFromString(invalidJson, "StartFruit"));
}

TEST(JsonParsingTest, MissingNode) {
    // 创建执行器
    Pipeline::PipelineExecutor executor;
    
    // 缺少节点的JSON字符串
    const std::string missingNodeJson = R"({
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
        }
        // 缺少Orange和Banana节点
    })";
    
    // 测试从字符串加载流水线
    EXPECT_TRUE(executor.executeFromString(missingNodeJson, "StartFruit"));
    
    // 测试使用不存在的节点启动流水线
    EXPECT_FALSE(executor.executeFromString(missingNodeJson, "NonExistentNode"));
}
