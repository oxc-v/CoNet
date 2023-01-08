#include "utility.hpp"

#include <fcntl.h>
#include <sys/signalfd.h>

namespace conet {
namespace utility {

const int MAX_BUF_SIZE = 1024;

void SetNonBlocking(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1)
        return;

    flags |= O_NONBLOCK;

    fcntl(fd, F_SETFL, flags);
}

int Accept(int fd, tcp::Socket& sock, error::error_code& ec)
{
    sockaddr_storage addr;
    socklen_t len = sizeof(addr);
    int ret = accept4(fd, (sockaddr*)&addr, &len, SOCK_NONBLOCK);
    if (ret > 0) {
        sock.SetFd(ret);
        sock.SetRemote(ip::Endpoint(addr));
    } else {
        if (errno == EAGAIN)
            ec.assign(error::would_block);
        else
            ec.assign(error::operator_error);
    }

    return ret;
}

int ReadSome(int fd, std::string& buf, size_t len, error::error_code& ec)
{
    char tmp[MAX_BUF_SIZE];

try_again:
    int n = read(fd, tmp, std::min(static_cast<int>(len), MAX_BUF_SIZE));
    if (n < 0) {
        if (errno == EINTR)
            goto try_again;
        else if (errno == EAGAIN)
            ec.assign(error::would_block);
        else
            ec.assign(error::operator_error);
    } else if (n == 0) {
        ec.assign(error::connection_closed);
    } else {
        buf.append(tmp, n);
    }

    return n > 0 ? n : -1;
}

int Read(int fd, std::string& buf, size_t len, error::error_code& ec)
{
    int readn = 0;
    for (;;) {
        int n = ReadSome(fd, buf, len - readn, ec);
        if (n > 0) {
            readn += n;
            if (readn == len)
                break;
        } else { 
            if (ec == error::would_block && readn > 0)
                break;
            else
                return -1;
        }
    }

    return readn;
}

int ReadSignal(int fd, error::error_code& ec)
{
try_again:
    signalfd_siginfo signalInfo;
    auto ret = read(fd, &signalInfo, sizeof(signalfd_siginfo));
    if (ret == sizeof(signalfd_siginfo)) {
        return signalInfo.ssi_signo;
    } else {
        if (errno == EINTR)
            goto try_again;
        else if (errno == EAGAIN)
            ec.assign(error::would_block);
        else
            ec.assign(error::operator_error);
    }

    return -1;
}

int WriteSome(int fd, const char* buf, size_t len, error::error_code& ec)
{
try_again:
    int n = write(fd, buf, len);
    if (n < 0) {
        if (errno == EINTR)
            goto try_again;
        else if (errno == EAGAIN)
            ec.assign(error::would_block);
        else
            ec.assign(error::operator_error);
    } else if (n == 0) {
        ec.assign(error::connection_closed);
    }

    return n > 0 ? n : -1;
}

int Write(int fd, const char* buf, size_t len, error::error_code& ec)
{
    int writen = 0;
    for (;;) {
        int n = WriteSome(fd, buf + writen, len - writen, ec);
        if (n > 0) {
            writen += n;
            if (writen == len)
                break;
        } else {
            if (ec == error::would_block && writen > 0)
                break;
            else
                return -1;
        }
    }

    return writen;
}

} // namespace utility
} // namespace conet