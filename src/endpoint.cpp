#include "endpoint.hpp"

#include <cstring>

namespace conet {
namespace ip {

Endpoint::Endpoint() noexcept
{
    data_.v4.family = AF_INET;
    data_.v4.sin_port = 0;
    data_.v4.sin_addr.s_addr = INADDR_ANY;
}

Endpoint::Endpoint(const sockaddr_storage& addr) noexcept
{
    if (addr.ss_family == AF_INET) {
        data_.v4.family = AF_INET;
        data_.v4.sin_port = ((sockaddr_in*)&addr)->sin_port;
        data_.v4.sin_addr.s_addr = ((sockaddr_in*)&addr)->sin_addr.s_addr;
    } else {
        data_.v6.family = AF_INET6;
        data_.v6.sin6_flowinfo = ((sockaddr_in6*)&addr)->sin6_flowinfo;
        data_.v6.sin6_port = ((sockaddr_in6*)&addr)->sin6_port;
        data_.v6.sin6_scope_id = ((sockaddr_in6*)&addr)->sin6_scope_id;
        memcpy(data_.v6.sin6_addr.s_addr, ((sockaddr_in6*)&addr)->sin6_addr.s6_addr, 16);
    }
}

Endpoint::Endpoint(const Endpoint& ep) noexcept
    : data_(ep.data_)
{
}

Endpoint& Endpoint::operator=(const Endpoint& ep) noexcept
{
    data_ = ep.data_;
    return *this;
}

Endpoint::Endpoint(int family, uint16_t port) noexcept
{
    if (family == AF_INET) {
        data_.v4.family = AF_INET;
        data_.v4.sin_port = htons(port);
        data_.v4.sin_addr.s_addr = INADDR_ANY;
    } else {
        data_.v6.family = AF_INET6;
        data_.v6.sin6_flowinfo = 0;
        data_.v6.sin6_port = htons(port);
        data_.v6.sin6_scope_id = 0;
        memset(data_.v6.sin6_addr.s_addr, 0, sizeof(data_.v6.sin6_addr.s_addr));
    }
}

Endpoint::Endpoint(const conet::ip::Address& addr, uint16_t port) noexcept
    : data_()
{
    if (addr.IsV4() == AF_INET) {
        data_.v4.family = AF_INET;
        data_.v4.sin_port = htons(port);
        data_.v4.sin_addr.s_addr = addr.v4().ToUint();
    } else {
        data_.v6.family = AF_INET6;
        data_.v6.sin6_flowinfo = 0;
        data_.v6.sin6_port = htons(port);
        data_.v6.sin6_scope_id = addr.v6().ScopeId();

        Address_v6::bytes_type bytes = addr.v6().ToBytes();
        memcpy(data_.v6.sin6_addr.s_addr, bytes.data(), 16);
    }
}

uint16_t Endpoint::Port() const noexcept
{
    if (IsV4())
        return ntohs(data_.v4.sin_port);
    else
        return ntohs(data_.v6.sin6_port);
}

Address Endpoint::Addr() const noexcept
{
    if (IsV4())
        return Address_v4(data_.v4.sin_addr.s_addr);
    
    Address_v6::bytes_type bytes;
    memcpy(bytes.data(), data_.v6.sin6_addr.s_addr, 16);
    return Address_v6(bytes, data_.v6.sin6_scope_id);
}

bool Endpoint::IsV4() const noexcept
{
    return data_.base.family == AF_INET;
}

int Endpoint::Family() const noexcept
{
    return data_.base.family;
}

std::string Endpoint::ToString() const
{
    if (IsV4()) {
        char buf[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &data_.v4.sin_addr, buf, INET_ADDRSTRLEN);
        return std::string(buf) + ":" + std::to_string(Port());
    } else {
        char buf[INET6_ADDRSTRLEN];
        inet_ntop(AF_INET6, &data_.v6.sin6_addr, buf, INET6_ADDRSTRLEN);
        return std::string(buf) + ":" + std::to_string(Port());
    }
}

} // namespace ip
} // namespace conet