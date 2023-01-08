/**
 * 封装系统信号集合，提供一些便捷的操作
 */

#pragma once

#include <unordered_set>
#include <functional>

#include "event_loop.hpp"

namespace conet {

namespace awaiter {
    struct SignalAwaiter;
} // namespace awaiter

namespace signal {

class SignalSet
{
public:
    SignalSet(EventLoop& loop);
    ~SignalSet();

    /**
     * @brief 添加信号，在下一次调用AyncWait时生效
     * @param signo 信号编号
     */
    void Add(int signo);

    /**
     * @brief 删除信号，在下一次调用AyncWait时生效
     * @param signo 信号编号
     */
    void Remove(int signo);

    /**
     * @brief 判断是否包含某个信号
     * @param signo 信号编号
     * @return 
     */
    bool Contains(int signo) const;

    /**
     * @brief 清空信号集合，在下一次调用AyncWait时生效
     */
    void Clear();

    /**
     * @brief 获取信号集合
     * @return 
     */
    const std::unordered_set<int>& GetSignals() const;

    /**
     * @brief 异步等待信号触发
     * @param ec
     * @return 
     */
    awaiter::SignalAwaiter AsyncWait(error::error_code& ec);

private:
    EventLoop& loop_;
    channel::Channel channel_;

    // 信号集合
    std::unordered_set<int> signals_;  

    // 信号处理函数
    std::function<void(int sig, error::error_code ec)> handler_; 
};

} // namespace signal
} // namespace conet