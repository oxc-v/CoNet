/**
 * Acceptor class，用于建立并接收接受客户端的连接 
 */

#pragma once

#include "endpoint.hpp"
#include "error.hpp"
#include "socket.hpp"
#include "channel.hpp"
#include "event_loop.hpp"

namespace conet {

namespace awaiter {
    struct AcceptAwaiter;
}

namespace tcp {

class Acceptor
{
public:
    Acceptor(EventLoop& loop, const ip::Endpoint& ep);

    /**
     * @brief 异步接受客户端连接
     * @param s
     * @param ec 
     * @return 
     */
    awaiter::AcceptAwaiter AsyncAccept(Socket& s, error::error_code& ec);

    /**
     * @brief 设置是否重用地址
     * @param on 
     */
    void SetReuseAddress(bool on);

    /**
     * @brief 绑定端口
     * @return 
     */
    bool Bind();

    /**
     * @brief 监听套接字
     * @return 
     */
    bool Listen();

private:
    int fd_;
    ip::Endpoint local_;
    channel::Channel channel_;
    EventLoop& loop_;
};

} // namespace tcp
} // namespace conet