/**
 * co_await 
 */

#pragma once

#include <sys/signalfd.h>
#include <iostream>

#include "awaitable.hpp"
#include "channel.hpp"
#include "utility.hpp"
#include "signal_set.hpp"

namespace conet {
namespace awaiter {

struct AcceptAwaiter
{
    tcp::Socket& sock_;
    error::error_code& ec_;
    channel::Channel& channel_;

    bool ready_ = true;

    bool await_ready()
    {
        auto ret = utility::Accept(channel_.Fd(), sock_, ec_);
        if (ec_ == error::would_block) {
            ready_ = false;
            ec_.clear();
        }

        return ready_;
    }

    void await_suspend(awaitable::coroutine_handle handle)
    {
        // 设置读事件处理器
        channel_.SetReadHandler(handle);
    }

    void await_resume() const
    {
        // 重置读事件处理器
        channel_.SetReadHandler(nullptr);

        // 终止事件循环、关闭套接字等操作会将错误码设置到 channel 中，
        // 并且会立即执行读写回调，因此这里需要检查错误码。
        auto error = channel_.ErrorCode();
        if (error) {
            ec_.assign(error.value());
            return;
        }

        if (!ready_)
            utility::Accept(channel_.Fd(), sock_, ec_);
    }
};

struct ReadAwaiter
{
    std::string& buf_;
    size_t len_;
    error::error_code& ec_;
    channel::Channel& channel_;
    bool readAll_ = true; // 是否读取所有数据

    bool ready_ = true;
    size_t readn_ = 0;
    
    bool await_ready() noexcept
    {
        char tmp[len_];
        int ret = 0;

        if (readAll_)
            ret = utility::Read(channel_.Fd(), tmp, len_, ec_);
        else
            ret = utility::ReadSome(channel_.Fd(), tmp, len_, ec_);

        if (ret < 0) {
            if (ec_ == error::would_block) {
                ready_ = false;
                ec_.clear();
            }
        } else {
            readn_ = ret;
            buf_.append(tmp, ret);
        }

        return ready_;
    }

    void await_suspend(awaitable::coroutine_handle handle) noexcept
    {
        channel_.SetReadHandler(handle);
    }

    size_t await_resume() noexcept
    {
        // 重置读事件处理器
        channel_.SetReadHandler(nullptr);

        // 终止事件循环、关闭套接字等操作会将错误码设置到 channel 中，
        // 并且会立即执行读写回调，因此这里需要检查错误码。
        auto error = channel_.ErrorCode();
        if (error) {
            ec_.assign(error.value());
            return 0;
        }

        if (!ready_) {
            char tmp[len_];
            int ret = 0;

            if (readAll_)
                ret = utility::Read(channel_.Fd(), tmp, len_, ec_);
            else
                ret = utility::ReadSome(channel_.Fd(), tmp, len_, ec_);

            if (ret > 0) {
                readn_ = ret;
                buf_.append(tmp, ret);
            }
        }

        return readn_;
    }
};

struct WriteAwaiter
{
    const char* buf_;
    size_t len_;
    error::error_code& ec_;
    channel::Channel& channel_;
    bool writeAll_ = true; // 是否写入所有数据

    bool ready_ = true;
    size_t writen_ = 0;

    bool await_ready() noexcept
    {
        int ret = 0;
        if (writeAll_)
            ret = utility::Write(channel_.Fd(), buf_, len_, ec_);
        else
            ret = utility::WriteSome(channel_.Fd(), buf_, len_, ec_);

        if (ret < 0) {
            if (ec_ == error::would_block) {
                ready_ = false;
                ec_.clear();
            }
        } else {
            writen_ = ret;
        }

        return ready_;
    }

    void await_suspend(awaitable::coroutine_handle handle) noexcept
    {
        channel_.SetWriteHandler(handle);
    }

    size_t await_resume() noexcept
    {
        // 重置写事件处理器
        channel_.SetWriteHandler(nullptr);

        // 终止事件循环、关闭套接字等操作会将错误码设置到 channel 中，
        // 并且会立即执行读写回调，因此这里需要检查错误码。
        auto error = channel_.ErrorCode();
        if (error) {
            ec_.assign(error.value());
            return 0;
        }

        if (!ready_) {
            int ret = 0;
            if (writeAll_)
                ret = utility::Write(channel_.Fd(), buf_, len_, ec_);
            else
                ret = utility::WriteSome(channel_.Fd(), buf_, len_, ec_);

            if (ret > 0)
                writen_ = ret;
        }

        return writen_;
    }
};

struct SocketAwaiter
{
    tcp::Socket::WaitType wt_;
    error::error_code& ec_;
    channel::Channel& channel_;

    constexpr bool await_ready() noexcept
    {
        return false;
    }

    void await_suspend(awaitable::coroutine_handle handle) noexcept
    {
        if (wt_ == tcp::Socket::socket_closed)
            channel_.SetCloseHandler(handle);
        else if (wt_ == tcp::Socket::socket_error)
            channel_.SetErrorHandler(handle);
    }

    void await_resume() const noexcept
    {
        // 终止事件循环、关闭套接字等操作会将错误码设置到 channel 中，
        // 并且会立即执行读写回调，因此这里需要检查错误码。
        auto error = channel_.ErrorCode();
        if (error) {
            ec_.assign(error.value());
            return;
        }

        if (wt_ == tcp::Socket::socket_closed)
            channel_.SetCloseHandler(nullptr);
        else if (wt_ == tcp::Socket::socket_error)
            channel_.SetErrorHandler(nullptr);
    }
};

struct SignalAwaiter
{
    channel::Channel& channel_;
    error::error_code& ec_;
    signal::SignalSet& sigset_;

    int signal_ = -1;
    bool ready_ = true;

    bool await_ready() noexcept
    {
        if (ec_)
            return ready_;

        signalfd_siginfo signalInfo;
        auto ret = utility::Read(channel_.Fd(), &signalInfo, sizeof(signalfd_siginfo), ec_);
        if (ret == sizeof(signalfd_siginfo)) {
            if (sigset_.Contains(signalInfo.ssi_signo))
                signal_ = signalInfo.ssi_signo;
            else
                ready_ = false;
        } else {
            if (ec_ == error::would_block) {
                ready_ = false;
                ec_.clear();
            }
        }

        return ready_;
    }

    void await_suspend(awaitable::coroutine_handle handle) noexcept
    {
        channel_.SetReadHandler(handle);
    }

    int await_resume() noexcept
    {
        channel_.SetReadHandler(nullptr);

        // 终止事件循环、关闭套接字等操作会将错误码设置到 channel 中，
        // 并且会立即执行读写回调，因此这里需要检查错误码。
        auto error = channel_.ErrorCode();
        if (error) {
            ec_.assign(error.value());
            return -1;
        }

        if (!ready_) {
            signalfd_siginfo signalInfo;
            auto ret = utility::Read(channel_.Fd(), &signalInfo, sizeof(signalfd_siginfo), ec_);
            if (ret == sizeof(signalfd_siginfo))
                signal_ = signalInfo.ssi_signo;
        }

        return signal_;
    }
};

struct BasicTimerAwaiter
{
    channel::Channel& channel_;
    error::error_code& ec_;
    bool expired_;

    bool ready_ = true;

    bool await_ready() noexcept
    {
        if (ec_ || expired_)
            return ready_;

        uint64_t exp;
        auto ret = utility::Read(channel_.Fd(), &exp, sizeof(uint64_t), ec_);
        if (ret != sizeof(uint64_t)) {
            if (ec_) {
                if (ec_ == error::would_block) {
                    ready_ = false;
                    ec_.clear();
                } 
            } else {
                ec_.assign(error::operator_error);
            }
        }

        return ready_;
    }

    void await_suspend(awaitable::coroutine_handle handle) noexcept
    {
        channel_.SetReadHandler(handle);
    }

    void await_resume() noexcept
    {
        channel_.SetReadHandler(nullptr);

        // 终止事件循环、关闭套接字等操作会将错误码设置到 channel 中，
        // 并且会立即执行读写回调，因此这里需要检查错误码。
        auto error = channel_.ErrorCode();
        if (error)
            ec_.assign(error.value());
    }
};

} // namespace awaiter
} // namespace conet