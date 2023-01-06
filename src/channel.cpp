#include "channel.hpp"

#include <sys/epoll.h>
#include <unistd.h>
#include <iostream>

namespace conet {
namespace channel {

Channel::Channel(int fd)
    : fd_(fd),
      events_(0),
      isClosed_(false)
{
}

int Channel::Fd() const noexcept
{
    return fd_;
}

void Channel::SetFd(int fd) noexcept
{
    fd_ = fd;
    
    if (fd_ != -1)
        ec_.clear();
}

error::error_code Channel::ErrorCode() const noexcept
{
    return ec_;
}

void Channel::SetErrorCode(const error::error_code& ec) noexcept
{
    ec_ = ec;
}

int Channel::Events() const noexcept
{
    return events_;
}

void Channel::SetEvents(int events) noexcept
{
    events_ = events;
}

void Channel::HandleEvents(int events)
{
    if (events & EPOLLRDHUP) {
        std::cout << "connection close event." << std::endl;
        isClosed_ = true;
        RunCloseHandler();
    }

    if (events & EPOLLIN || events & EPOLLPRI) {
        std::cout << "read event." << std::endl;
        RunReadHandler();
    }

    if (events & EPOLLOUT) {
        std::cout << "write event." << std::endl;
        RunWriteHandler();
    }

    if (events & EPOLLERR) {
        std::cout << "error event." << std::endl;
        RunErrorHandler();
    }
}

void Channel::SetReadHandler(awaitable::coroutine_handle handler) noexcept
{
    if (ec_ && handler) {
        handler.resume();
        return;
    }

    readHandler_ = handler;
}

void Channel::SetWriteHandler(awaitable::coroutine_handle handler) noexcept
{
    if (ec_ && handler) {
        handler.resume();
        return;
    }

    writeHandler_ = handler;
}

void Channel::SetCloseHandler(awaitable::coroutine_handle handler) noexcept
{
    if (ec_ && handler) {
        handler.resume();
        return;
    }

    closeHandler_ = handler;
}

void Channel::SetErrorHandler(awaitable::coroutine_handle handler) noexcept
{
    if (ec_ && handler) {
        handler.resume();
        return;
    }

    errorHandler_ = handler;
}

bool Channel::IsOpen() const noexcept
{
    return !isClosed_;
}

void Channel::RunReadHandler()
{
    if (readHandler_)
        readHandler_.resume();
}

void Channel::RunWriteHandler()
{
    if (writeHandler_)
        writeHandler_.resume();
}

void Channel::RunCloseHandler()
{
    if (closeHandler_)
        closeHandler_.resume();
}

void Channel::RunErrorHandler()
{
    if (errorHandler_)
        errorHandler_.resume();
}

void Channel::Close()
{
    if (fd_ == -1)
        return;

    ec_.assign(error::channel_closed);

    RunReadHandler();
    RunWriteHandler();
    RunCloseHandler();
    RunErrorHandler();

    close(fd_);
    fd_ = -1;
}

} // namespace channel
} // namespace conet