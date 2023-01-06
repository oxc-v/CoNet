#include "acceptor.hpp"

#include <sys/socket.h>
#include <sys/epoll.h>
#include <cstring>

#include "awaitable.hpp"
#include "awaiter.hpp"

namespace conet {
namespace tcp {

Acceptor::Acceptor(EventLoop& loop, const ip::Endpoint& ep)
    : fd_(-1),
      loop_(loop),
      local_(ep)
{
    fd_ = socket(ep.Family(), SOCK_STREAM, 0);
    if (fd_ < 0) {
        perror("socket");
        exit(1);
    }

    // 设置fd为非阻塞
    utility::SetNonBlocking(fd_);
}

bool Acceptor::Bind()
{
    sockaddr_storage addr;
    if (local_.IsV4()) {
        sockaddr_in* addr_v4 = (sockaddr_in*)&addr;
        addr_v4->sin_family = AF_INET;
        addr_v4->sin_port = htons(local_.Port());
        addr_v4->sin_addr.s_addr = local_.Addr().v4().ToUint();
    } else {
        sockaddr_in6* addr_v6 = (sockaddr_in6*)&addr;
        addr_v6->sin6_family = AF_INET6;
        addr_v6->sin6_port = htons(local_.Port());
        addr_v6->sin6_scope_id = local_.Addr().v6().ScopeId();
        memcpy(addr_v6->sin6_addr.s6_addr, local_.Addr().v6().ToBytes().data(), 16);
    }

    if (bind(fd_, (sockaddr*)&addr, sizeof(addr)) < 0)
        return false;

    return true;
}

bool Acceptor::Listen()
{
    if (listen(fd_, 1024) < 0)
        return false;

    channel_.SetFd(fd_);
    channel_.SetEvents(EPOLLIN | EPOLLET);
    loop_.AddChannel(&channel_);

    return true;
}

awaiter::AcceptAwaiter Acceptor::AsyncAccept(Socket& s, error::error_code& ec)
{
    return awaiter::AcceptAwaiter(s, ec, channel_);
}

void Acceptor::SetReuseAddress(bool on)
{
    int optval = on ? 1 : 0;
    setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
}

} // namespace tcp
} // namespace conet