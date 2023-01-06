/**
 * 负责监听IO事件，将事件分发给对应的handler
 */

#pragma once

#include <sys/epoll.h>
#include <unordered_map>
#include <vector>

#include "channel.hpp"

namespace conet {

class EventLoop
{
public:
    EventLoop();
    ~EventLoop();

    EventLoop(const EventLoop&) = delete;
    EventLoop& operator=(const EventLoop&) = delete;

    /**
     * @brief 运行事件循环
     */
    void Run();

    /**
     * @brief 停止事件循环
     */
    void Stop() noexcept;

    /**
     * @brief 添加channel
     * @param channel 
     */
    void AddChannel(channel::Channel* channel);

    /**
     * @brief 移除channel
     * @param fd
     */
    void RemoveChannel(int fd);

private:
    bool running_;
    int epollFd_;
    
    // epoll事件
    std::vector<epoll_event> events_;

    // fd -> channel
    std::unordered_map<int, channel::Channel*> channels_;
};

} // namespace conet