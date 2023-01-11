/**
 * 一个异步的简单定时器
 */

#pragma once

#include <sys/timerfd.h>
#include <chrono>

#include "channel.hpp"
#include "event_loop.hpp"
#include "awaiter.hpp"

namespace conet {
namespace basic_timer {

template <typename Clock>
class BasicTimer
{
public:
    using time_point = std::chrono::time_point<Clock>;
    using duration = Clock::duration;

    BasicTimer(const BasicTimer&) = delete;
    BasicTimer(BasicTimer&&) = delete;
    BasicTimer& operator=(const BasicTimer&) = delete;
    BasicTimer& operator=(BasicTimer&&) = delete;

    explicit BasicTimer(EventLoop& loop)
        : loop_(loop),
          expiry_(Clock::now()),
          isCancel_(false)
    {
        fd_ = timerfd_create(Clock::is_steady ? CLOCK_MONOTONIC : CLOCK_REALTIME, TFD_NONBLOCK);
        if (fd_ < 0) {
            perror("timerfd_create");
            exit(1);
        }
        channel_.SetFd(fd_);
        channel_.SetEvents(EPOLLIN | EPOLLET);
        loop_.AddChannel(&channel_);
    }

    // 以绝对时间的方式设置到期时间
    BasicTimer(EventLoop& loop, const time_point& expiry)
        : loop_(loop),
          expiry_(expiry),
          isCancel_(false)
    {
        fd_ = timerfd_create(Clock::is_steady ? CLOCK_MONOTONIC : CLOCK_REALTIME, TFD_NONBLOCK);
        if (fd_ < 0) {
            perror("timerfd_create");
            exit(1);
        }
        channel_.SetFd(fd_);
        channel_.SetEvents(EPOLLIN | EPOLLET);
        loop_.AddChannel(&channel_);
    }

    // 以相对时间的方式设置到期时间
    BasicTimer(EventLoop& loop, const duration& expiry)
        : loop_(loop),
          expiry_(Clock::now() + expiry),
          isCancel_(false)
    {
        fd_ = timerfd_create(Clock::is_steady ? CLOCK_MONOTONIC : CLOCK_REALTIME, TFD_NONBLOCK);
        if (fd_ < 0) {
            perror("timerfd_create");
            exit(1);
        }
        channel_.SetFd(fd_);
        channel_.SetEvents(EPOLLIN | EPOLLET);
        loop_.AddChannel(&channel_);
    }

    ~BasicTimer()
    {
        if (!isCancel_)
            Cancel();

        std::cout << "BasicTimer::~BasicTimer()" << std::endl;
    }

    /**
     * @brief 获取定时器的到期时间（绝对时间），定时器是否超时不影响获取的结果
     * @return 
     */
    time_point Expiry() const
    {
        return expiry_;
    }

    /**
     * @brief 设置定时器的到期时间，立即执行已有的异步操作，并置错误码error::operator_cancel
     * @param expiry 到期时间（绝对时间）
     */
    void ExpiryAt(const time_point& expiry)
    {
        if (isCancel_)
            return;

        expiry_ = expiry;
        channel_.SetErrorCode(error::operator_cancel);
        channel_.RunReadHandler();
        channel_.SetErrorCode(error::error_code()); // 重置错误码
    }

    /**
     * @brief 设置定时器的到期时间，立即执行已有的异步操作，并置错误码error::operator_cancel
     * @param expiry 到期时间（相对时间）
     */
    void ExpiryAfter(const duration& expiry)
    {
        if (isCancel_)
            return;

        expiry_ = Clock::now() + expiry;
        channel_.SetErrorCode(error::operator_cancel);
        channel_.RunReadHandler();
        channel_.SetErrorCode(error::error_code()); // 重置错误码
    }

    /**
     * @brief 异步等待定时器到期
     * @param ec 错误码
     */
    awaiter::BasicTimerAwaiter AsyncWait(error::error_code& ec)
    { 
        // 定时器已不可用
        if (isCancel_) {
            ec.assign(error::operator_cancel);
            return awaiter::BasicTimerAwaiter(channel_, ec, true);
        }

        bool isTimeout = (expiry_ >= Clock::now()) ? false : true;
        if (!isTimeout) {
            auto interval = expiry_ - Clock::now();
            struct itimerspec new_value;
            new_value.it_value.tv_sec = interval.count() / 1000000000;
            new_value.it_value.tv_nsec = interval.count() % 1000000000;
            new_value.it_interval.tv_sec = 0;
            new_value.it_interval.tv_nsec = 0;

            int ret = timerfd_settime(fd_, 0, &new_value, nullptr);
            if (ret < 0) 
                ec.assign(error::operator_error);
        }

        return awaiter::BasicTimerAwaiter(channel_, ec, isTimeout);
    }

    /**
     * @brief 取消定时器，立即执行已有的异步操作，并置错误码error::channel_cancel，
     * 定时器将无法继续使用
     */
    void Cancel()
    {
        if (isCancel_) 
            return;

        isCancel_ = true;
        channel_.Close();
        loop_.RemoveChannel(fd_);
        close(fd_);
    }

private:
    EventLoop& loop_;
    time_point expiry_;
    channel::Channel channel_;
    bool isCancel_;
    int fd_;
};

} // namespace basic_timer
} // namespace conet