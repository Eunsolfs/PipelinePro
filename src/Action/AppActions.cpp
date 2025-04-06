#include "Pipeline/Action/AppActions.h"
#include "Pipeline/RecognitionResult.h"
#include <iostream>

namespace Pipeline {

// StartAppAction实现
StartAppAction::StartAppAction() : Action(ActionType::StartApp) {
}

bool StartAppAction::parseConfig(const nlohmann::json& config) {
    // 解析包名
    if (config.contains("package")) {
        m_package = config["package"].get<std::string>();
    }
    
    return true;
}

bool StartAppAction::execute(const RecognitionResult& result) {
    // 这里应该实现实际的启动应用操作
    // 由于实际的启动应用操作不在本库的范围内，这里只是一个示例实现
    
    if (m_package.empty()) {
        return false;
    }
    
    // 处理变量表达式
    std::string processedPackage = m_package;
    if (m_variableManager && (m_package.find("%") != std::string::npos || 
                             m_package.find("[") != std::string::npos || 
                             m_package.find("{") != std::string::npos)) {
        processedPackage = m_variableManager->processLogString(m_package);
    }
    
    // 执行启动应用
    // 在实际实现中，这里应该调用实际的启动应用函数
    std::cout << "Starting app: " << processedPackage << std::endl;
    
    return true; // 假设启动应用成功
}

// StopAppAction实现
StopAppAction::StopAppAction() : Action(ActionType::StopApp) {
}

bool StopAppAction::parseConfig(const nlohmann::json& config) {
    // 解析包名
    if (config.contains("package")) {
        m_package = config["package"].get<std::string>();
    }
    
    return true;
}

bool StopAppAction::execute(const RecognitionResult& result) {
    // 这里应该实现实际的停止应用操作
    // 由于实际的停止应用操作不在本库的范围内，这里只是一个示例实现
    
    if (m_package.empty()) {
        return false;
    }
    
    // 处理变量表达式
    std::string processedPackage = m_package;
    if (m_variableManager && (m_package.find("%") != std::string::npos || 
                             m_package.find("[") != std::string::npos || 
                             m_package.find("{") != std::string::npos)) {
        processedPackage = m_variableManager->processLogString(m_package);
    }
    
    // 执行停止应用
    // 在实际实现中，这里应该调用实际的停止应用函数
    std::cout << "Stopping app: " << processedPackage << std::endl;
    
    return true; // 假设停止应用成功
}

} // namespace Pipeline
