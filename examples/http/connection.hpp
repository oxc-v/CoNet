/**
 * 处理连接的类 
 */

#pragma once

#include "conet.hpp"
#include "response.hpp"

#include <memory>
#include <iostream>

class Connection : public std::enable_shared_from_this<Connection>
{
public:
    Connection(const Connection&) = delete;
    Connection& operator=(const Connection&) = delete;

    Connection(conet::EventLoop& loop);
    ~Connection() { std::cout << "~Connection." << std::endl; }

    /**
     * @brief 获取socket
     * @return 
     */
    conet::tcp::Socket& Socket() noexcept;

    /**
     * @brief 处理连接
     */
    void Start();

    /**
     * @brief 停止连接
     */
    void Stop();

private:
    /**
     * @brief 处理读
     */
    conet::awaitable::Awaitable HandleRead();

    /**
     * @brief 处理写
     */
    conet::awaitable::Awaitable HandleWrite(const Response& resp);

private:
    conet::tcp::Socket sock_;
};