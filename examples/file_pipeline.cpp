#include <PipelineLib.h>
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    // 检查命令行参数
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <pipeline_json_file> <start_node>" << std::endl;
        return 1;
    }
    
    const char* pipelineFile = argv[1];
    const char* startNode = argv[2];
    
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
    
    // 执行流水线
    if (!PipelineExecuteFromFile(executor, pipelineFile, startNode)) {
        std::cerr << "Failed to execute pipeline from file: " << pipelineFile << std::endl;
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
