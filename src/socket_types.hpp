/**
 * 定义一些socket类型，提高通用性
 */

#pragma once

#include <cstdint>

namespace conet {
namespace types {

struct In4AddrType { 
    uint32_t s_addr; 
};

struct In6AddrType { 
    uint8_t s_addr[16]; 
};

struct SocketAddrType { 
    int family; 
};

struct SockaddrIn4Type { 
    int family;
    In4AddrType sin_addr; 
    uint16_t sin_port; 
};

struct SockaddrIn6Type { 
    int family;
    In6AddrType sin6_addr;
    uint16_t sin6_port;
    uint32_t sin6_flowinfo; 
    uint32_t sin6_scope_id; 
     
};

} // namespace types
} // namespace conet
