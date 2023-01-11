/**
 * 自定义错误码
 */

#pragma once

#include <system_error>
#include <string>

namespace conet {
namespace error {

enum ErrorCode
{
    ip_invalid = 1000, // 无效的IP地址
    connection_closed = 1001, // 连接已关闭
    operator_error = 1002, // 操作错误
    channel_closed = 1003, // channel已关闭
    would_block = 1004, // 阻塞
    try_again = 1005, // 重试
    operator_cancel = 1006 // 操作取消
};

class system_category : public std::error_category
{
public:
    const char* name() const noexcept override
    {
        return "conet catagory";
    }

    std::string message(int ec) const override
    {
        switch (ec) {
        case ip_invalid:
            return "Invalid ip address";
        case connection_closed:
            return "Connection closed";
        case operator_error:
            return "Operator error";
        case channel_closed:
            return "Channel closed";
        case would_block:
            return "Would block";
        case try_again:
            return "Try again";
        case operator_cancel:
            return "Operator cancel";
        default:
            return "Unknown error";
        }
    }
};

inline const system_category& error_category() noexcept
{
    static system_category s;
    return s;
}

class error_code : public std::error_code
{
public:
    error_code()
        : std::error_code(0, error_category())
    {
    }

    error_code(int ev)
        : std::error_code(ev, error_category())
    {
    }

    void assign(int ev) noexcept
    {
        std::error_code::assign(ev, error_category());
    }

    bool operator!() const noexcept
    {
        return value() == 0;
    }

    friend bool operator==(const error_code& e1, const error_code& e2)
    { 
        return e1.value() == e2.value() && e1.category() == e2.category(); 
    }

    friend bool operator!=(const error_code& e1, const error_code& e2)
    { 
        return !(e1 == e2); 
    }
};

} // namespace error
} // namespace conet