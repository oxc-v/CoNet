#include "event_loop_pool.hpp"

#include <thread>
#include <memory>

EventLoopPool::EventLoopPool(std::size_t size)
    : size_(size)
    , index_(0)
{
    for (std::size_t i = 0; i < size_; ++i)
        loops_.push_back(new conet::EventLoop());
}

EventLoopPool::~EventLoopPool()
{
    for (auto loop: loops_)
        delete loop;
}

conet::EventLoop& EventLoopPool::GetNextLoop()
{
    conet::EventLoop& loop = *loops_[index_];
    index_ = (index_ + 1) % size_;
    return loop;
}

void EventLoopPool::Start()
{
    std::vector<std::shared_ptr<std::thread>> threads;

    // 创建线程，启动事件循环
    for (std::size_t i = 0; i < size_; ++i) {
        threads.push_back(std::make_shared<std::thread>([this, i] {
            loops_[i]->Run();
        }));
    }

    // 等待线程结束
    for (auto& thread: threads)
        thread->join();
}

void EventLoopPool::Stop()
{
    // 停止事件循环
    for (auto loop: loops_)
        loop->Stop();
}