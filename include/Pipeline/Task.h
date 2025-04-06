#pragma once

#include "Pipeline/Common.h"

namespace Pipeline {

// 任务完成等待器，用于协程
struct PIPELINE_API TaskAwaiter {
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

// 任务类，用于基于协程的执行
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
    
    // 不可复制
    Task(const Task&) = delete;
    Task& operator=(const Task&) = delete;
    
    // 可移动
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

} // namespace Pipeline
