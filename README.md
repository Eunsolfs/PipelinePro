# Pipeline库

Pipeline库是一个用于解析和执行自定义任务流水线的C++库。它可以从JSON文件中加载流水线定义，并按照定义的顺序和条件执行相应的操作。

## 特性

- 使用C++20标准，包括协程、虚基类和派生类等现代C++特性
- 支持从JSON文件或字符串加载流水线定义
- 支持多种识别算法和动作类型
- 支持条件分支和错误处理
- 支持在流水线中定义和使用变量，包括条件判断和变量操作
- 可编译为DLL，提供C接口供其他语言调用

## 构建要求

- C++20兼容的编译器（如GCC 10+、Clang 10+、MSVC 2019+）
- CMake 3.15或更高版本
- nlohmann/json库（构建时会自动下载）

## 构建步骤

```bash
# 创建构建目录
mkdir build
cd build

# 配置
cmake ..

# 构建
cmake --build .

# 安装（可选）
cmake --install .
```

## 使用示例

```cpp
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

    // 从文件加载并执行流水线
    if (!PipelineExecuteFromFile(executor, "pipeline.json", "StartNode")) {
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
```

## 流水线JSON格式

流水线使用JSON格式定义，每个节点包含识别算法、执行动作和后继节点等属性。例如：

```json
{
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
}
```

## 许可证

本项目采用MIT许可证。详见LICENSE文件。
