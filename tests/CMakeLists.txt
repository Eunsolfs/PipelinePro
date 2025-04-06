cmake_minimum_required(VERSION 3.15)

# 添加GoogleTest
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.12.1
)
FetchContent_MakeAvailable(googletest)

# 测试：解析JSON
add_executable(test_json_parsing test_json_parsing.cpp)
target_link_libraries(test_json_parsing PRIVATE PipelineLib gtest gtest_main)
add_test(NAME test_json_parsing COMMAND test_json_parsing)

# 测试：节点执行
add_executable(test_node_execution test_node_execution.cpp)
target_link_libraries(test_node_execution PRIVATE PipelineLib gtest gtest_main)
add_test(NAME test_node_execution COMMAND test_node_execution)

# 测试：流水线执行
add_executable(test_pipeline_execution test_pipeline_execution.cpp)
target_link_libraries(test_pipeline_execution PRIVATE PipelineLib gtest gtest_main)
add_test(NAME test_pipeline_execution COMMAND test_pipeline_execution)
