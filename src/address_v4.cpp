#include "address_v4.hpp"

namespace conet {
namespace ip {

Address_v4::Address_v4() noexcept
{
    inet_pton(AF_INET, "127.0.0.1", &addr_);
}

Address_v4::Address_v4(uint32_t addr) noexcept
{
    addr_.s_addr = addr;
}

Address_v4::Address_v4(const Address_v4& other) noexcept
    : addr_(other.addr_) {}

Address_v4::Address_v4(Address_v4&& other) noexcept
    : addr_(other.addr_) {}

Address_v4& Address_v4::operator=(const Address_v4& other) noexcept
{
    addr_ = other.addr_;
    return *this;
}

Address_v4& Address_v4::operator=(Address_v4&& other) noexcept
{
    addr_ = other.addr_;
    return *this;
}

std::string Address_v4::ToString() const
{
    char buf[INET_ADDRSTRLEN];
    auto ptr = inet_ntop(AF_INET, &addr_, buf, sizeof(buf));
    if (ptr == nullptr)
        return std::string();

    return std::string(ptr);
}

std::string Address_v4::ToString(error::error_code& ec) const
{
    char buf[INET_ADDRSTRLEN];
    auto ptr = inet_ntop(AF_INET, &addr_, buf, sizeof(buf));
    if (ptr == nullptr) {
        ec.assign(error::ip_invalid);
        return std::string();
    }

    return std::string(ptr);
}

Address_v4 Address_v4::FromString(const std::string& str)
{
    Address_v4 addr;
    auto ret = inet_pton(AF_INET, str.c_str(), &addr.addr_);
    if (ret != 1)
        return Address_v4();

    return addr;
}

Address_v4 Address_v4::FromString(const std::string& str, error::error_code& ec)
{
    Address_v4 addr;
    auto ret = inet_pton(AF_INET, str.c_str(), &addr.addr_);
    if (ret != 1) 
        ec.assign(error::ip_invalid);

    return addr; 
}

uint32_t Address_v4::ToUint() const noexcept
{
    return addr_.s_addr;
}

} // namespace ip
} // namespace conet