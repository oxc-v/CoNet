/**
 * 一个IP端点类，主要包括IP地址，端口
 */

#pragma once

#include "address.hpp"
#include "socket_types.hpp"

namespace conet {
namespace ip {

class Endpoint 
{
public:
    Endpoint() noexcept;
    Endpoint(const sockaddr_storage& addr) noexcept;
    Endpoint(const Endpoint& ep) noexcept;
    Endpoint& operator=(const Endpoint& ep) noexcept;
    Endpoint(int family, uint16_t port) noexcept;
    Endpoint(const Address& addr, uint16_t port) noexcept;

    /**
     * @brief 返回一个端口号
     * @return 
     */
    uint16_t Port() const noexcept;

    /**
     * @brief 返回一个地址
     * @return 
     */
    Address Addr() const noexcept;

    /**
     * @brief 判断当前端点是否为IPv4
     * @return 
     */
    bool IsV4() const noexcept;

    /**
     * @brief 返回当前端点的地址族
     * @return 
     */
    int Family() const noexcept;

    /**
     * @brief 返回一个字符串类型的端口
     * @return 
     */
    std::string ToString() const;

private:
    union {
        conet::types::SocketAddrType base;
        conet::types::SockaddrIn4Type v4;
        conet::types::SockaddrIn6Type v6;
    } data_;
};

} // namespace ip
} // namespace conet