#include "event_loop.hpp"

#include <unistd.h>
#include <sys/signalfd.h>
#include <csignal>

#include "signal_set.hpp"

namespace conet {

const int MAX_EVENTS = 1024;
const int EPOLL_TIMEOUT = 1000;

EventLoop::EventLoop()
    : running_(true), 
      events_(MAX_EVENTS)
{
    epollFd_ = epoll_create(1);
    if (epollFd_ == -1) {
        perror("epoll_create");
        exit(1);
    }
}

EventLoop::~EventLoop()
{
    // 关闭文件描述符
    close(epollFd_);
}

void EventLoop::Run()
{
    while (running_) {
        int nfds = epoll_wait(epollFd_, &*events_.begin(), MAX_EVENTS, EPOLL_TIMEOUT);
        if (nfds == -1) {
            perror("epoll_wait");
            continue;
        }
        for (int i = 0; i < nfds; ++i) {
            int fd = events_[i].data.fd;

            if (channels_.find(fd) == channels_.end())
                continue;

            channels_[fd]->HandleEvents(events_[i].events);
        }
    }

    // 关闭所有channel
    for (auto [fd, channel] : channels_)
        channel->Close();

    // 清空channels
    channels_.clear();
}

void EventLoop::Stop() noexcept
{
    running_ = false;
}

void EventLoop::AddChannel(channel::Channel* channel)
{
    epoll_event ev;
    ev.data.fd = channel->Fd();
    ev.events = channel->Events();
    epoll_ctl(epollFd_, EPOLL_CTL_ADD, channel->Fd(), &ev);
    channels_[channel->Fd()] = channel;
}

void EventLoop::RemoveChannel(int fd)
{
    epoll_ctl(epollFd_, EPOLL_CTL_DEL, fd, nullptr);
    channels_.erase(fd);
}

} // namespace conet