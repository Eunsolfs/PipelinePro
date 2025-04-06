cmake_minimum_required(VERSION 3.15)
project(PipelineLib VERSION 1.0.0 LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 设置输出目录
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# 添加编译选项
if(MSVC)
    add_compile_options(/W4 /MP)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
else()
    add_compile_options(-Wall -Wextra -pedantic)
endif()

# 添加nlohmann/json库
find_package(nlohmann_json QUIET)
if(NOT nlohmann_json_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.11.2
    )
    FetchContent_MakeAvailable(nlohmann_json)
endif()

# 添加OpenCV
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

# 添加vision库
add_subdirectory(vision)

# 设置包含目录
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# 收集源文件
file(GLOB ACTION_SOURCES "src/Action/*.cpp")
file(GLOB RECOGNITION_SOURCES "src/Recognition/*.cpp")
file(GLOB MAIN_SOURCES "src/*.cpp")
set(SOURCES ${ACTION_SOURCES} ${RECOGNITION_SOURCES} ${MAIN_SOURCES})

# 收集头文件
file(GLOB_RECURSE HEADERS "include/*.h")

# 创建共享库
add_library(PipelineLib SHARED ${SOURCES} ${HEADERS})
target_compile_definitions(PipelineLib PRIVATE PIPELINE_EXPORTS)
target_link_libraries(PipelineLib PRIVATE nlohmann_json::nlohmann_json ${OpenCV_LIBS} vision)

# 安装目标
install(TARGETS PipelineLib
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

# 安装头文件
install(DIRECTORY include/ DESTINATION include)

# 添加示例程序
option(BUILD_EXAMPLES "Build example programs" ON)
if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# 添加测试
option(BUILD_TESTS "Build test programs" ON)
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
