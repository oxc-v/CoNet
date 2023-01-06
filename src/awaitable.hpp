/**
 * C++ 20 Coroutines 
 */

#pragma once

#include <coroutine>

namespace conet {
namespace awaitable {

struct promise;
struct Awaitable: std::coroutine_handle<promise> 
{
    using promise_type = struct promise;
};

struct promise {
    Awaitable get_return_object() { return {Awaitable::from_promise(*this)}; }
    std::suspend_never initial_suspend() noexcept { return {}; }
    std::suspend_never final_suspend() noexcept { return {}; }
    void return_void() {}
    void unhandled_exception() {}
};

using coroutine_handle = std::coroutine_handle<promise>;

} // namespace awaitable
} // namespace conet