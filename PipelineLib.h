#pragma once

#ifdef PIPELINE_EXPORTS
#define PIPELINE_API __declspec(dllexport)
#else
#define PIPELINE_API __declspec(dllimport)
#endif

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <optional>
#include <coroutine>
#include <future>
#include <variant>
#include <any>
#include <chrono>
#include <thread>
#include <regex>
#include <filesystem>
#include <iostream>

// Forward declarations
namespace nlohmann {
    template<typename T>
    class basic_json;
    
    using json = basic_json<>;
}

namespace Pipeline {

// Forward declarations
class Node;
class PipelineExecutor;

// Recognition types
enum class RecognitionType {
    DirectHit,
    TemplateMatch,
    FindColor,
    OCR,
    FindMultiColor,
    FindColorList,
    FindMultiColorList
};

// Action types
enum class ActionType {
    DoNothing,
    Click,
    Swipe,
    Key,
    Text,
    StartApp,
    StopApp,
    StopTask,
    Command
};

// Result of a recognition operation
struct RecognitionResult {
    bool success = false;
    std::vector<int> box = {0, 0, 0, 0}; // x, y, width, height
    double score = 0.0;
    std::string text;
    
    // Additional data that might be needed by specific recognition types
    std::any additionalData;
    
    operator bool() const { return success; }
};

// Base class for all recognition algorithms
class PIPELINE_API Recognition {
public:
    virtual ~Recognition() = default;
    
    // Pure virtual method to be implemented by derived classes
    virtual RecognitionResult recognize() = 0;
    
    // Factory method to create recognition objects based on type
    static std::unique_ptr<Recognition> create(RecognitionType type, const nlohmann::json& config);
    
    // Set inverse flag
    void setInverse(bool inverse) { m_inverse = inverse; }
    
    // Get the recognition type
    RecognitionType getType() const { return m_type; }
    
protected:
    Recognition(RecognitionType type) : m_type(type), m_inverse(false) {}
    
    RecognitionType m_type;
    bool m_inverse;
};

// Base class for all actions
class PIPELINE_API Action {
public:
    virtual ~Action() = default;
    
    // Pure virtual method to be implemented by derived classes
    virtual bool execute(const RecognitionResult& result) = 0;
    
    // Factory method to create action objects based on type
    static std::unique_ptr<Action> create(ActionType type, const nlohmann::json& config);
    
    // Get the action type
    ActionType getType() const { return m_type; }
    
protected:
    Action(ActionType type) : m_type(type) {}
    
    ActionType m_type;
};

// Node class representing a single node in the pipeline
class PIPELINE_API Node {
public:
    Node(const std::string& name);
    ~Node() = default;
    
    // Initialize node from JSON configuration
    bool initialize(const nlohmann::json& config, std::map<std::string, std::shared_ptr<Node>>& allNodes);
    
    // Execute the node's recognition and action
    RecognitionResult executeRecognition();
    bool executeAction(const RecognitionResult& result);
    
    // Getters
    const std::string& getName() const { return m_name; }
    const std::vector<std::string>& getNextNodes() const { return m_nextNodes; }
    const std::vector<std::string>& getInterruptNodes() const { return m_interruptNodes; }
    const std::vector<std::string>& getOnErrorNodes() const { return m_onErrorNodes; }
    bool isEnabled() const { return m_enabled; }
    uint32_t getTimeout() const { return m_timeout; }
    uint32_t getPreDelay() const { return m_preDelay; }
    uint32_t getPostDelay() const { return m_postDelay; }
    bool isFocused() const { return m_focus; }
    
private:
    std::string m_name;
    std::unique_ptr<Recognition> m_recognition;
    std::unique_ptr<Action> m_action;
    std::vector<std::string> m_nextNodes;
    std::vector<std::string> m_interruptNodes;
    std::vector<std::string> m_onErrorNodes;
    bool m_enabled = true;
    bool m_inverse = false;
    uint32_t m_timeout = 20000; // Default 20 seconds
    uint32_t m_preDelay = 200;  // Default 200ms
    uint32_t m_postDelay = 200; // Default 200ms
    bool m_focus = false;
};

// Task completion awaitable for coroutines
struct TaskAwaiter {
    bool await_ready() const noexcept { return false; }
    void await_suspend(std::coroutine_handle<> handle) const noexcept { m_handle = handle; }
    void await_resume() const noexcept {}
    
    void resume() const {
        if (m_handle) {
            m_handle.resume();
        }
    }
    
    mutable std::coroutine_handle<> m_handle;
};

// Task class for coroutine-based execution
class PIPELINE_API Task {
public:
    struct promise_type {
        Task get_return_object() { return Task{std::coroutine_handle<promise_type>::from_promise(*this)}; }
        std::suspend_never initial_suspend() { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        void return_void() {}
        void unhandled_exception() { std::terminate(); }
    };
    
    explicit Task(std::coroutine_handle<promise_type> handle) : m_handle(handle) {}
    ~Task() {
        if (m_handle) {
            m_handle.destroy();
        }
    }
    
    // Non-copyable
    Task(const Task&) = delete;
    Task& operator=(const Task&) = delete;
    
    // Movable
    Task(Task&& other) noexcept : m_handle(other.m_handle) {
        other.m_handle = nullptr;
    }
    
    Task& operator=(Task&& other) noexcept {
        if (this != &other) {
            if (m_handle) {
                m_handle.destroy();
            }
            m_handle = other.m_handle;
            other.m_handle = nullptr;
        }
        return *this;
    }
    
private:
    std::coroutine_handle<promise_type> m_handle;
};

// Pipeline class to manage the execution of nodes
class PIPELINE_API Pipeline {
public:
    Pipeline();
    ~Pipeline();
    
    // Load pipeline from JSON file
    bool loadFromFile(const std::string& filePath);
    
    // Load pipeline from JSON string
    bool loadFromString(const std::string& jsonString);
    
    // Get a node by name
    std::shared_ptr<Node> getNode(const std::string& name) const;
    
    // Execute the pipeline starting from a specific node
    Task execute(const std::string& startNodeName);
    
    // Stop the current execution
    void stop();
    
private:
    std::map<std::string, std::shared_ptr<Node>> m_nodes;
    bool m_running = false;
    
    // Helper method to parse JSON
    bool parseJson(const nlohmann::json& json);
};

// PipelineExecutor class to manage the execution of pipelines
class PIPELINE_API PipelineExecutor {
public:
    PipelineExecutor();
    ~PipelineExecutor();
    
    // Load and execute a pipeline from a file
    bool executeFromFile(const std::string& filePath, const std::string& startNodeName);
    
    // Load and execute a pipeline from a string
    bool executeFromString(const std::string& jsonString, const std::string& startNodeName);
    
    // Stop the current execution
    void stop();
    
    // Set a callback for node execution events
    using NodeCallback = std::function<void(const std::string& nodeName, bool success)>;
    void setNodeCallback(NodeCallback callback);
    
private:
    std::unique_ptr<Pipeline> m_pipeline;
    NodeCallback m_nodeCallback;
    Task m_currentTask;
};

// Exported functions
extern "C" {
    // Initialize the library
    PIPELINE_API bool PipelineInit();
    
    // Clean up the library
    PIPELINE_API void PipelineCleanup();
    
    // Create a pipeline executor
    PIPELINE_API PipelineExecutor* PipelineCreateExecutor();
    
    // Destroy a pipeline executor
    PIPELINE_API void PipelineDestroyExecutor(PipelineExecutor* executor);
    
    // Execute a pipeline from a file
    PIPELINE_API bool PipelineExecuteFromFile(PipelineExecutor* executor, const char* filePath, const char* startNodeName);
    
    // Execute a pipeline from a string
    PIPELINE_API bool PipelineExecuteFromString(PipelineExecutor* executor, const char* jsonString, const char* startNodeName);
    
    // Stop pipeline execution
    PIPELINE_API void PipelineStop(PipelineExecutor* executor);
}

} // namespace Pipeline
