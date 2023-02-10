/**
 * 事件循环池，事先创建好一定数量的事件循环，然后按照轮询的方式分配给每个连接，以此来实现多线程的并发
 */

#pragma once

#include <vector>

#include "event_loop.hpp"

class EventLoopPool
{
public:
    explicit EventLoopPool(std::size_t size);
    ~EventLoopPool();

    EventLoopPool(const EventLoopPool&) = delete;
    EventLoopPool& operator=(const EventLoopPool&) = delete;

    /**
     * @brief 获取下一个事件循环
     * @return 
     */
    conet::EventLoop& GetNextLoop();

    /**
     * @brief 启动事件循环
     */
    void Start();

    /**
     * @brief 停止事件循环
     */
    void Stop();

private:
    std::size_t size_; // 事件循环数量
    std::size_t index_; // 当前事件循环索引
    std::vector<conet::EventLoop*> loops_; // 事件循环列表

};