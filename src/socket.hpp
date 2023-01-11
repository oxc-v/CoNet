/**
 * Socket类，封装了socket的基本操作
 */

#pragma once

#include "channel.hpp"
#include "endpoint.hpp"
#include "event_loop.hpp"

namespace conet {

namespace awaiter {
    struct SocketAwaiter;
    struct ReadAwaiter;
    struct WriteAwaiter;
}

namespace tcp {

class Socket
{
public:
    enum WaitType
    {
        socket_closed = 0,
        socket_error
    };

    Socket(EventLoop& loop);
    ~Socket();

    /**
     * @brief 判断套接字是否关闭
     * @return 
     */
    bool IsOpen() const noexcept;

    /**
     * @brief 关闭套接字
     * @return 
     */
    void Close() noexcept;

    /**
     * @brief 获取套接字
     * @return 
     */
    int Fd() const noexcept;

    /**
     * @brief 设置套接字
     * @return 
     */
    void SetFd(int fd) noexcept;

    /**
     * @brief 设置远程地址
     * @return 
     */
    void SetRemote(const ip::Endpoint& ep) noexcept;

    /**
     * @brief 获取远程地址
     * @return 
     */
    ip::Endpoint Remote() const noexcept;

    /**
     * @brief 异步等待指定事件发生
     * @param wt
     * @param ec
     */
    awaiter::SocketAwaiter AsyncWait(WaitType wt, error::error_code& ec) noexcept;

    /**
     * @brief 异步读取数据，读满len个字节或者遇到错误，不够len个字节返回error::would_block
     * @param buf 
     * @param ec 
     * @return 
     */
    awaiter::ReadAwaiter AsyncRead(std::string& buf, size_t len, error::error_code& ec);

    /**
     * @brief 异步读取数据，尽量读取len个字符
     * @param buf 
     * @param ec 
     */
    awaiter::ReadAwaiter AsyncReadSome(std::string& buf, size_t len, error::error_code& ec);

    /**
     * @brief 异步写入数据，写满len个字节或者遇到错误，写缓冲区满返回error::would_block
     * @param buf 
     * @param len
     * @param ec 
     * @return 
     */
    awaiter::WriteAwaiter AsyncWrite(const std::string& buf, size_t len, error::error_code& ec);

    /**
     * @brief 异步写入数据，尽量写入len个字符
     * @param buf 
     * @param len
     * @param ec 
     */
    awaiter::WriteAwaiter AsyncWriteSome(const std::string& buf, size_t len, error::error_code& ec);

private:
    int fd_;
    ip::Endpoint remote_;
    channel::Channel channel_;
    EventLoop& loop_;
};

} // namespace tcp
} // namespace conet