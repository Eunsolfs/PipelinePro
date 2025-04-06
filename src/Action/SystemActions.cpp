#include "Pipeline/Action/SystemActions.h"
#include "Pipeline/RecognitionResult.h"
#include <iostream>

namespace Pipeline {

// CommandAction实现
CommandAction::CommandAction() : Action(ActionType::Command) {
}

bool CommandAction::parseConfig(const nlohmann::json& config) {
    // 解析执行程序
    if (config.contains("exec")) {
        m_exec = config["exec"].get<std::string>();
    }

    // 解析参数
    if (config.contains("args")) {
        m_args = config["args"].get<std::vector<std::string>>();
    }

    // 解析是否分离
    if (config.contains("detach")) {
        m_detach = config["detach"].get<bool>();
    }
    
    return true;
}

bool CommandAction::execute(const RecognitionResult& result) {
    // 这里应该实现实际的命令执行操作
    // 由于实际的命令执行操作不在本库的范围内，这里只是一个示例实现
    
    if (m_exec.empty()) {
        return false;
    }
    
    // 处理变量表达式
    std::string processedExec = m_exec;
    if (m_variableManager && (m_exec.find("%") != std::string::npos || 
                             m_exec.find("[") != std::string::npos || 
                             m_exec.find("{") != std::string::npos)) {
        processedExec = m_variableManager->processLogString(m_exec);
    }
    
    std::vector<std::string> processedArgs;
    for (const auto& arg : m_args) {
        std::string processedArg = arg;
        if (m_variableManager && (arg.find("%") != std::string::npos || 
                                 arg.find("[") != std::string::npos || 
                                 arg.find("{") != std::string::npos)) {
            processedArg = m_variableManager->processLogString(arg);
        }
        processedArgs.push_back(processedArg);
    }
    
    // 执行命令
    // 在实际实现中，这里应该调用实际的命令执行函数
    std::cout << "Executing command: " << processedExec;
    for (const auto& arg : processedArgs) {
        std::cout << " " << arg;
    }
    std::cout << (m_detach ? " (detached)" : "") << std::endl;
    
    return true; // 假设命令执行成功
}

} // namespace Pipeline
