/**  
 * IPv6地址 
 */

#pragma once

#include <arpa/inet.h>
#include <string_view>
#include <string>
#include <array>

#include "error.hpp"
#include "socket_types.hpp"

namespace conet {
namespace ip {

class Address_v6
{ 
public:
    using bytes_type = std::array<uint8_t, 16>;

    Address_v6() noexcept;
    Address_v6(const bytes_type& bytes, uint32_t scopeId = 0) noexcept;
    Address_v6(const Address_v6& other) noexcept;
    Address_v6(Address_v6&& other) noexcept;
    Address_v6& operator=(const Address_v6& other) noexcept;
    Address_v6& operator=(Address_v6&& other) noexcept;
    
    /**
     * @brief 获取IPv6范围ID
     * @return 
     */
    uint32_t ScopeId() const noexcept;

    /**
     * @brief 设置IPv6范围ID
     * @param id 
     */
    void SetScopeId(uint32_t id) noexcept;

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
    static Address_v6 FromString(const std::string& str);

    /**
     * @brief 根据点分十进制形式的字符串创建Address_v4
     * @param str
     * @param ec 
     * @return 
     */
    static Address_v6 FromString(const std::string& str, error::error_code& ec);

    /**
     * @brief 返回一个网络字节顺序的IP地址
     * @return 
     */
    bytes_type ToBytes() const noexcept;

private:
    conet::types::In6AddrType addr_;

    // IPv6地址作用域ID
    uint32_t scopeId_;
};

} // namespace ip
} // namespace conet