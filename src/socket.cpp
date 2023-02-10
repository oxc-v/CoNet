#include "socket.hpp"

#include <unistd.h>
#include <fcntl.h>

#include "error.hpp"
#include "awaiter.hpp"

namespace conet {
namespace tcp {

Socket::Socket(EventLoop& loop)
    : loop_(loop)
{
}

Socket::~Socket()
{
    if (IsOpen())
        Close();
}

int Socket::Fd() const noexcept
{
    return fd_;
}

void Socket::SetFd(int fd) noexcept
{
    fd_ = fd;

    channel_.SetFd(fd);
    channel_.SetEvents(EPOLLIN | EPOLLET | EPOLLRDHUP | EPOLLERR | EPOLLPRI | EPOLLOUT);
    loop_.AddChannel(&channel_);
}

void Socket::SetRemote(const ip::Endpoint& ep) noexcept
{
    remote_ = ep;
}

ip::Endpoint Socket::Remote() const noexcept
{
    return remote_;
}

bool Socket::IsOpen() const noexcept
{
    return channel_.IsOpen();
}

void Socket::Close() noexcept
{
    channel_.Close();
    loop_.RemoveChannel(fd_);
}

awaiter::SocketAwaiter Socket::AsyncWait(WaitType wt, error::error_code& ec) noexcept
{
    return awaiter::SocketAwaiter{wt, ec, channel_};
}

awaiter::ReadAwaiter Socket::AsyncReadSome(std::string& buf, size_t len, error::error_code& ec)
{
    return awaiter::ReadAwaiter(buf, len, ec, channel_, false);
}

awaiter::ReadAwaiter Socket::AsyncRead(std::string& buf, size_t len, error::error_code& ec)
{
    return awaiter::ReadAwaiter(buf, len, ec, channel_);
}

awaiter::WriteAwaiter Socket::AsyncWriteSome(const std::string& buf, size_t len, error::error_code& ec)
{
    return awaiter::WriteAwaiter(buf.c_str(), len, ec, channel_, false);
}

awaiter::WriteAwaiter Socket::AsyncWrite(const std::string& buf, size_t len, error::error_code& ec)
{
    return awaiter::WriteAwaiter(buf.c_str(), len, ec, channel_);
}

} // namespace tcp
} // namespace conet