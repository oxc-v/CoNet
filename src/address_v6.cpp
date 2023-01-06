#include "address_v6.hpp"

#include <cstring>

namespace conet {
namespace ip {

Address_v6::Address_v6() noexcept
    : scopeId_(0)
{
    inet_pton(AF_INET6, "::1", &addr_);
}

Address_v6::Address_v6(const Address_v6& other) noexcept
  : addr_(other.addr_),
    scopeId_(other.scopeId_)
{
}

Address_v6::Address_v6(Address_v6&& other) noexcept
  : addr_(other.addr_),
    scopeId_(other.scopeId_)
{
}

Address_v6::Address_v6(const bytes_type& bytes, uint32_t scopeId) noexcept
  : scopeId_(scopeId)
{
    memcpy(addr_.s_addr, bytes.data(), 16);
}

Address_v6& Address_v6::operator=(const Address_v6& other) noexcept
{
  addr_ = other.addr_;
  scopeId_ = other.scopeId_;
  return *this;
}

Address_v6& Address_v6::operator=(Address_v6&& other) noexcept
{
  addr_ = other.addr_;
  scopeId_ = other.scopeId_;
  return *this;
}

uint32_t Address_v6::ScopeId() const noexcept
{
    return scopeId_;
}

void Address_v6::SetScopeId(uint32_t id) noexcept
{
    scopeId_ = id;
}

std::string Address_v6::ToString() const
{
    char buf[INET6_ADDRSTRLEN];
    auto ptr = inet_ntop(AF_INET6, &addr_, buf, sizeof(buf));
    if (ptr == nullptr)
        return std::string();

    return std::string(ptr);
}

std::string Address_v6::ToString(error::error_code& ec) const
{
    char buf[INET6_ADDRSTRLEN];
    auto ptr = inet_ntop(AF_INET6, &addr_, buf, sizeof(buf));
    if (ptr == nullptr) {
        ec.assign(error::ip_invalid);
        return std::string();
    }

    return std::string(ptr);
}

Address_v6 Address_v6::FromString(const std::string& str)
{
    Address_v6 addr;
    auto ret = inet_pton(AF_INET6, str.c_str(), &addr.addr_);
    if (ret != 1)
        return Address_v6();

    return addr;
}

Address_v6 Address_v6::FromString(const std::string& str, error::error_code& ec)
{
    Address_v6 addr;
    auto ret = inet_pton(AF_INET6, str.c_str(), &addr.addr_);
    if (ret != 1) 
        ec.assign(error::ip_invalid);

    return addr;
}

Address_v6::bytes_type Address_v6::ToBytes() const noexcept
{
    bytes_type bytes;
    memcpy(bytes.data(), addr_.s_addr, 16);
    return bytes;
}

} // namespace ip
} // namespace conet