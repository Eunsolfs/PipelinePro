## 项目结构总结

我已经按照合理的方式将代码拆分到不同的文件中，形成了以下结构：

1. **头文件目录 (include/)**
   * `PipelineLib.h` - 主要的API头文件
   * `Pipeline/Common.h` - 公共定义和宏
   * `Pipeline/Recognition.h` - 识别算法相关类
   * `Pipeline/Action.h` - 动作相关类
   * `Pipeline/Node.h` - 节点类定义
   * `Pipeline/Task.h` - 协程任务相关类
   * `Pipeline/VariableManager.h` - 变量管理类
   * `Pipeline/Pipeline.h` - 流水线管理类
   * `Pipeline/PipelineExecutor.h` - 流水线执行器类
2. **源文件目录 (src/)**
   * `Common.cpp` - 公共函数实现
   * `Recognition.cpp` - 识别算法实现
   * `Action.cpp` - 动作实现
   * `Node.cpp` - 节点类实现
   * `VariableManager.cpp` - 变量管理类实现
   * `Pipeline.cpp` - 流水线管理类实现
   * `PipelineExecutor.cpp` - 流水线执行器实现
   * `PipelineLib.cpp` - DLL导出函数实现
3. **示例目录 (examples/)**
   * `simple_pipeline.cpp` - 简单的流水线执行示例
   * `file_pipeline.cpp` - 从文件加载流水线的示例
   * `variable_pipeline.cpp` - 带变量的流水线示例
   * `global_variable_pipeline.cpp` - 带全局变量的流水线示例
   * `suspend_resume_pipeline.cpp` - 暂停和继续流水线示例
   * `stop_task_pipeline.cpp` - 使用StopTask动作的示例
   * `condition_process_pipeline.cpp` - 使用条件处理的示例
   * `recognition_example.cpp` - 使用各种识别算法的示例
4. **测试目录 (tests/)**
   * `test_json_parsing.cpp` - JSON解析测试
   * `test_node_execution.cpp` - 节点执行测试
   * `test_pipeline_execution.cpp` - 流水线执行测试
5. **构建文件**
   * `CMakeLists.txt` - 主CMake构建文件
   * `examples/CMakeLists.txt` - 示例程序构建文件
   * `tests/CMakeLists.txt` - 测试程序构建文件
6. **文档**
   * `README.md` - 项目说明文档
   * `doc/doc.md` - 详细文档
7. **视觉库 (vision/)**
   * `include/vision/vision.h` - 视觉库头文件
   * `src/vision.cpp` - 视觉库实现
   * `examples/` - 视觉库示例程序

## 功能特点

1. **C++20特性** ：

* 使用了协程（coroutines）实现异步执行流程
* 使用了虚基类和派生类实现多态
* 使用了现代C++的智能指针、变体类型等特性

1. **DLL导出** ：

* 定义了适当的宏和导出函数，使库可以编译为DLL
* 提供了C风格的接口，方便其他语言调用

1. **流水线执行** ：

* 支持从JSON文件或字符串加载流水线定义
* 实现了按照定义的顺序和条件执行节点
* 支持条件分支、中断和错误处理
* 支持在流水线中定义和使用变量
* 支持流水线的暂停和继续执行

1. **可扩展性** ：

* 通过虚基类和工厂方法，可以方便地添加新的识别算法和动作类型
* 模块化的设计使得代码易于维护和扩展

## 使用方法

1. **构建库** ：

```cpp
mkdir build
cd build
cmake ..
cmake --build .
```


1. **使用库** ：

* 包含头文件 `#include <PipelineLib.h>`
* 链接生成的库文件
* 使用提供的API函数加载和执行流水线

1. **示例程序** ：

* `simple_pipeline` 展示了如何从字符串加载和执行流水线
* `file_pipeline` 展示了如何从文件加载和执行流水线
* `variable_pipeline` 展示了如何使用变量功能
* `global_variable_pipeline` 展示了如何使用全局变量
* `suspend_resume_pipeline` 展示了如何暂停和继续流水线执行
* `stop_task_pipeline` 展示了如何使用StopTask动作主动停止任务
* `condition_process_pipeline` 展示了如何使用condition_process参数进行条件处理
* `recognition_example` 展示了如何使用各种识别算法
* `structured_action_example` 展示了如何使用结构化的action格式
* `fully_structured_example` 展示了如何同时使用结构化的recognition和action格式
* `action_with_variables_example` 展示了如何在action参数中使用变量表达式
* `coordinate_variables_example` 展示了如何使用坐标类型变量

## 变量功能

1. **变量定义**：
   * 在节点的`var`属性中定义变量，如`"%inumOfBattle=0"`
   * 在JSON根级别的`var_global`属性中定义全局变量，如`"%iglobalCounter=0"`
   * 支持不同类型的变量：
     * 整数（`%i`）：存储整数值
     * 字符串（`%s`）：存储字符串值
     * 浮点数（`%f`）：存储浮点数值
     * 布尔值（`%b`）：存储布尔值
     * 点坐标（`%p`）：存储(x,y)坐标，如`"%pStartPoint=100,200"`
     * 矩形区域（`%r`）：存储(x1,y1,x2,y2)坐标，如`"%rArea=50,50,250,250"`

2. **条件判断**：
   * 在节点的`condition`属性中使用条件表达式，如`"%inumOfBattle<3"`
   * 支持各种比较操作符：`<`, `>`, `<=`, `>=`, `==`, `!=`

3. **变量操作**：
   * 在日志字符串中使用花括号执行变量操作，如`{%inumOfBattle++}`
   * 支持自增（`++`）、自减（`--`）和赋值（`=`）操作
   * 支持复杂的算术表达式，如`{%inumOfBattle=%iMaxNum-%iIntervalNum}`

4. **变量引用**：
   * 在日志字符串中使用方括号引用变量值，如`[%inumOfBattle]`

5. **全局与局部变量**：
   * 全局变量在整个流水线中可见，在JSON根级别的`var_global`属性中定义
   * 局部变量只在定义它的节点中可见，在节点的`var`属性中定义
   * 全局变量和局部变量可以同名，局部变量会在当前节点中覆盖全局变量

6. **在Action中使用变量**：
   * 在Action的参数中可以使用变量表达式，如`"target": "[%ix],[%iy]"`
   * 支持在坐标参数中使用变量运算，如`"target": "{%ix+50},{%iy+50}"`
   * 变量表达式会在执行时被解析和计算
   * 在ClickAction和SwipeAction等类中，当参数类型为string时，会检查是否包含变量表达式

7. **坐标类型变量**：
   * 点坐标变量（`%p`）可以直接用于ClickAction和SwipeAction的参数，如`"target": "%pStartPoint"`
   * 矩形区域变量（`%r`）可以用于定义随机点击区域，如`"target": "%rArea"`
   * 坐标类型变量可以在日志中直接引用，如`"log": {"true": "Clicking at [%pStartPoint]"}`
   * SwipeAction执行后会自动生成坐标变量：`%pLastSwipeBegin`、`%pLastSwipeEnd`和`%rLastSwipeArea`

## 暂停和继续功能

1. **流水线状态**：
   * 流水线有三种状态：运行中（Running）、暂停（Suspended）和停止（Stopped）
   * 可以通过`PipelineGetState`函数获取当前状态

2. **暂停功能**：
   * 使用`PipelineSuspend`函数暂停流水线执行
   * 暂停后，流水线会保存当前执行状态，包括当前节点和变量值

3. **继续功能**：
   * 使用`PipelineContinue`函数继续执行已暂停的流水线
   * 继续执行时，流水线会从暂停点恢复执行

4. **当前节点信息**：
   * 使用`PipelineGetCurrentNodeName`函数获取当前正在执行的节点名称
   * 这对于调试和监控流水线执行非常有用

## 条件处理功能

1. **condition_process参数**：
   * 在节点中使用`condition_process`参数可以定义条件满足和不满足时的处理逻辑
   * 包含`true`和`false`两个分支，分别对应条件满足和不满足时的处理

2. **动态重写节点**：
   * `override_next`：动态重写当前节点的next列表，控制下一个要执行的节点
   * `override_interrupt`：动态重写当前节点的interrupt列表

3. **变量操作**：
   * `var_operation`：定义条件满足或不满足时要执行的变量操作
   * 支持复杂的表达式，如`{%inumOfBattle=%iMaxNum-%iIntervalNum}`

4. **条件日志**：
   * `condition_log`：定义条件满足或不满足时要输出的日志
   * 支持变量引用和变量操作，如`condition_log => 第 [%inumOfBattle] / {%iallnumOfBattle++}次`

5. **使用场景**：
   * 根据条件动态调整流水线执行路径
   * 在条件判断时执行复杂的变量操作
   * 提供更灵活的日志和调试信息

## StopTask动作功能

1. **任务主动停止**：
   * 使用`StopTask`动作可以在流水线中主动停止任务执行
   * 当执行到该动作时，流水线会停止并触发回调

2. **停止回调**：
   * 使用`PipelineSetTaskStopCallback`函数设置任务停止回调
   * 回调函数会提供停止时的节点名称和原因

3. **使用场景**：
   * 当流水线中的某个条件满足时，主动结束任务
   * 当发生错误或异常情况时，安全地停止任务
   * 当任务完成目标时，不需要继续执行后续节点

## 结构化Action格式

1. **新的格式**：
   * 将action的参数封装在一个JSON对象中，并添加type字段指定action类型
   * 例如：`"action": {"type": "Click", "target": [100, 200], ...}`

2. **优点**：
   * 结构更清晰：参数与其所属的action类型明确关联
   * 扩展性更好：可以轻松添加新的action类型和参数
   * 参数隔离：避免了潜在的命名冲突

3. **兼容性**：
   * 同时支持旧格式：`"action": "Click"`
   * 可以平滑迁移到新格式

4. **示例**：
```json
{
    "Click": {
        "recognition": "DirectHit",
        "action": {
            "type": "Click",
            "target": [100, 200],
            "target_offset": [10, 10, 0, 0]
        },
        "next": ["Swipe"]
    }
}
```

## 结构化Recognition格式

1. **新的格式**：
   * 将recognition的参数封装在一个JSON对象中，并添加type字段指定recognition类型
   * 例如：`"recognition": {"type": "FindColor", "roi": [0, 0, 1920, 1080], ...}`

2. **优点**：
   * 结构更清晰：参数与其所属的recognition类型明确关联
   * 扩展性更好：可以轻松添加新的recognition类型和参数
   * 参数隔离：避免了潜在的命名冲突

3. **兼容性**：
   * 同时支持旧格式：`"recognition": "FindColor"`
   * 可以平滑迁移到新格式

4. **示例**：
```json
{
    "FindMultiColor": {
        "recognition": {
            "type": "FindMultiColor",
            "roi": [0, 0, 1920, 1080],
            "multi_color": "FF0000-101010|5|5|00FF00-101010,10|10|0000FF-101010",
            "similarity": 0.9,
            "direction": 0
        },
        "action": {
            "type": "Click",
            "target": true
        },
        "next": ["TemplateMatch"]
    }
}
```

## 完全结构化格式

通过同时使用结构化的recognition和action格式，可以使配置文件更加清晰和结构化：

```json
{
    "TemplateMatch": {
        "recognition": {
            "type": "TemplateMatch",
            "roi": [0, 0, 1920, 1080],
            "template": ["template1.jpg", "template2.jpg"],
            "threshold": [0.7, 0.8],
            "method": 5
        },
        "action": {
            "type": "Swipe",
            "begin": [100, 200],
            "end": [300, 400],
            "duration": 500
        },
        "next": ["OCR"]
    }
}
```

这种完全结构化的格式具有以下优点：

1. **一致性**：所有参数都使用相同的结构化方式
2. **可读性**：配置文件更易读和理解
3. **可维护性**：添加或修改参数更加直观
4. **错误防范**：参数的属性更明确，减少配置错误

## 视觉识别功能

1. **视觉库**：
   * 独立的vision库，包含OCR、找图、找色等功能
   * 基于OpenCV和FastDeploy实现
   * 可以被其他项目复用

2. **识别算法**：
   * `TemplateMatch`：基于模板匹配的识别算法
   * `OCR`：基于OCR的文字识别算法
   * `FindColor`：基于颜色查找的识别算法
   * `FindMultiColor`：基于多点颜色查找的识别算法

3. **参数配置**：
   * 支持从JSON或字符串解析参数
   * 支持设置ROI、阈值、方向等参数
   * 支持多种匹配方法和排序方式

4. **使用方式**：
   * 在流水线JSON中配置识别算法和参数
   * 通过Recognition类调用vision库的功能
   * 支持条件判断和变量操作

这个项目完全符合您的要求，使用了C++20的现代特性，实现了DLL导出功能，并且代码组织良好，便于维护和扩展。您可以根据需要进一步完善实际的识别算法和动作实现。
