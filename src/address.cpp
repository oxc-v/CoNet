#include "address.hpp"

#include <stdexcept>

namespace conet {
namespace ip {

Address::Address() noexcept
    : type_(IPv4),
      addr_v4_(),
      addr_v6_() 
{
}

Address::Address(const Address_v4& addr) noexcept
    : type_(IPv4),
      addr_v4_(addr),
      addr_v6_()
{
}

Address::Address(const Address_v6& addr) noexcept
    : type_(IPv6),
      addr_v4_(),
      addr_v6_(addr)
{
}

Address::Address(const Address& other) noexcept
    : type_(other.type_),
      addr_v4_(other.addr_v4_),
      addr_v6_(other.addr_v6_)
{
}

Address::Address(Address&& other) noexcept
    : type_(other.type_),
      addr_v4_(other.addr_v4_),
      addr_v6_(other.addr_v6_)
{
}

Address& Address::operator=(const Address& other) noexcept
{
    type_ = other.type_;
    addr_v4_ = other.addr_v4_;
    addr_v6_ = other.addr_v6_;
    return *this;
}

Address& Address::operator=(Address&& other) noexcept
{
    type_ = other.type_;
    addr_v4_ = other.addr_v4_;
    addr_v6_ = other.addr_v6_;
    return *this;
}

bool Address::IsV4() const noexcept
{
    return type_ == IPv4;
}

bool Address::IsV6() const noexcept
{
    return type_ == IPv6;
}

Address_v4 Address::v4() const
{
    if (type_ != IPv4)
        throw std::invalid_argument("Address is not IPv4");
    
    return addr_v4_;
}

Address_v6 Address::v6() const
{
    if (type_ != IPv6)
        throw std::invalid_argument("Address is not IPv6");

    return addr_v6_;
}

std::string Address::ToString() const
{
    if (type_ == IPv4)
        return addr_v4_.ToString();
    else
        return addr_v6_.ToString();
}

std::string Address::ToString(std::error_code& ec) const
{
    if (type_ == IPv4)
        return addr_v4_.ToString(ec);
    else
        return addr_v6_.ToString(ec);
}

Address Address::FromString(const std::string& str, std::error_code& ec)
{
    std::error_code ec;
    auto addr_v6 = Address_v6::FromString(str, ec);
    if (!ec)
        return Address(addr_v6);

    auto addr_v4 = Address_v4::FromString(str, ec);
    if (!ec)
        return Address(addr_v4);
    
    return Address();
}

Address Address::FromString(const std::string& str)
{
    std::error_code ec;
    auto addr = Address::FromString(str, ec);

    return addr;
}

} // namespace ip
} // namespace conet