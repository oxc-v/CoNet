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
};

class SystemCategory : public std::error_category
{
public:
    const char* name() const noexcept override;
    std::string message(int ec) const override;
};

void AssignError(std::error_code& ec, int ev)
{   
    static SystemCategory s;
    ec.assign(ev, s);
}

} // namespace error
} // namespace conet