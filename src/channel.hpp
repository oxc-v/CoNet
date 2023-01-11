/**
 * Channel类负责管理一个连接的事件调度。
 */

#pragma once

#include <list>

#include "awaitable.hpp"
#include "error.hpp"

namespace conet {
namespace channel {

class Channel
{
public:
    Channel(int fd = -1);
    Channel(const Channel&) = delete;
    Channel& operator=(const Channel&) = delete;

    /**
     * @brief 获取文件描述符
     * @return 
     */
    int Fd() const noexcept;

    /**
     * @brief 设置文件描述符
     * @param fd 
     */
    void SetFd(int fd) noexcept;

    /**
     * @brief 获取错误码
     * @return 
     */
    error::error_code ErrorCode() const noexcept;

    /**
     * @brief 设置错误码
     * @param code 
     */
    void SetErrorCode(const error::error_code& ec) noexcept;

    /**
     * @brief 获取事件
     * @return 
     */
    int Events() const noexcept;

    /**
     * @brief 设置事件
     * @param events 
     */
    void SetEvents(int events) noexcept;

    /**
     * @brief 处理epoll事件
     */
    void HandleEvents(int events);

    /**
     * @brief 设置读事件的协程句柄
     * @param handler 
     */
    void SetReadHandler(awaitable::coroutine_handle handler) noexcept;

    /**
     * @brief 设置写事件的协程句柄
     * @param handler 
     */
    void SetWriteHandler(awaitable::coroutine_handle handler) noexcept;

    /**
     * @brief 设置关闭事件的协程句柄，服务端主动关闭连接时不触发
     * @param handler 
     */
    void SetCloseHandler(awaitable::coroutine_handle handler) noexcept;

    /**
     * @brief 设置错误事件的协程句柄
     * @param handler 
     */
    void SetErrorHandler(awaitable::coroutine_handle handler) noexcept;

    /**
     * @brief 获取连接是否关闭
     * @return 
     */
    bool IsOpen() const noexcept;

    /**
     * @brief 关闭通道，并执行所有协程句柄
     */
    void Close();

    /**
     * @brief 运行读事件的协程句柄
     */
    void RunReadHandler();

    /**
     * @brief 运行写事件的协程句柄
     */
    void RunWriteHandler();

    /**
     * @brief 运行关闭事件的协程句柄
     */
    void RunCloseHandler();

    /**
     * @brief 运行错误事件的协程句柄
     */
    void RunErrorHandler();

private:
    int fd_;
    int events_;
    bool isClosed_;
    error::error_code ec_;

    // 读事件的协程句柄
    awaitable::coroutine_handle readHandler_;

    // 写事件的协程句柄
    awaitable::coroutine_handle writeHandler_;

    // 关闭事件的协程句柄
    awaitable::coroutine_handle closeHandler_;

    // 错误事件的协程句柄
    awaitable::coroutine_handle errorHandler_;
};

} // namespace channel
} // namespace conet