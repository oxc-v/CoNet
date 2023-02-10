/**
 * http server example 
 */

#pragma once

#include "conet.hpp"
#include "event_loop_pool.hpp"
#include "connection.hpp"

#include <string>
#include <memory>

class Server
{
public:
    Server(const std::string& ip, int port, std::size_t loopSize = 5);
    ~Server();

    /**
     * @brief 启动服务器
     */
    void Start();

private:
    /**
     * @brief 处理信号
     */
    conet::awaitable::Awaitable HandleSignals();

    /**
     * @brief 处理连接
     */
    conet::awaitable::Awaitable HandleConnection();

private:
    EventLoopPool loopPool_;
    conet::signal::SignalSet signals_;
    conet::tcp::Acceptor acceptor_;
    std::shared_ptr<Connection> conn_;
};