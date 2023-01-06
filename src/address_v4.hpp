/**  
 * IPv4地址 
 */

#pragma once

#include <arpa/inet.h>
#include <string_view>
#include <string>

#include "error.hpp"
#include "socket_types.hpp"

namespace conet {
namespace ip {

class Address_v4
{ 
public:
    Address_v4() noexcept;
    Address_v4(uint32_t addr) noexcept;
    Address_v4(const Address_v4& other) noexcept;
    Address_v4(Address_v4&& other) noexcept;
    Address_v4& operator=(const Address_v4& other) noexcept;
    Address_v4& operator=(Address_v4&& other) noexcept;

    /**
     * @brief 返回一个点分十进制格式的字符串形式的地址
     * @return
     */
    std::string ToString() const;

    /**
     * @brief 返回一个点分十进制格式的字符串形式的地址
     * @param ec
     * @return
     */
    std::string ToString(error::error_code& ec) const;

    /**
     * @brief 根据点分十进制形式的字符串创建Address_v4
     * @param str
     * @return 
     */
    static Address_v4 FromString(const std::string& str);

    /**
     * @brief 根据点分十进制形式的字符串创建Address_v4
     * @param str
     * @param ec 
     * @return 
     */
    static Address_v4 FromString(const std::string& str, error::error_code& ec);

    /**
     * @brief 返回一个网络字节顺序的IP地址
     * @return 
     */
    uint32_t ToUint() const noexcept;

private:
    conet::types::In4AddrType addr_; 
};

} // namespace ip
} // namespace conet