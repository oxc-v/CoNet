/**
 * Address对IPv4和IPv6地址的封装，实现一个与IP版本无关的地址类。
 */

#pragma once

#include <string>
#include <string_view>

#include "address_v4.hpp"
#include "address_v6.hpp"

namespace conet {
namespace ip {

class Address
{
public:
    Address() noexcept;

    Address(const Address_v4& addr) noexcept;

    Address(const Address_v6& addr) noexcept;

    Address(const Address& other) noexcept;

    Address(Address&& other) noexcept;

    Address& operator=(const Address& other) noexcept;

    Address& operator=(Address&& other) noexcept;

    /**
     * @brief 判断是否为IPv4地址
     * @return 
     */
    bool IsV4() const noexcept;

    /**
     * @brief 判断是否为IPv6地址
     * @return 
     */
    bool IsV6() const noexcept;

    /**
     * @brief 返回一个IPv4地址
     * @return 
     */
    Address_v4 v4() const;

    /**
     * @brief 返回一个IPv6地址
     * @return 
     */
    Address_v6 v6() const;

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
    std::string ToString(std::error_code& ec) const;

    /**
     * @brief 根据点分十进制形式的字符串创建Address
     * @param str 
     * @return 
     */
    static Address FromString(const std::string& str);

    /**
     * @brief 根据点分十进制形式的字符串创建Address_v4
     * @param str
     * @param ec 
     * @return 
     */
    static Address FromString(const std::string& str, std::error_code& ec);

private:
    // IP地址类型
    enum { IPv4, IPv6 } type_;

    // IPv4地址
    Address_v4 addr_v4_;

    // IPv6地址
    Address_v6 addr_v6_;
};

} // namespace ip
} // namespace conet