#include "error.hpp"

namespace conet {
namespace error {

const char* SystemCategory::name() const noexcept
{
    return "conet catagory";
}

std::string SystemCategory::message(int ec) const
{
    switch (ec) {
    case ip_invalid:
        return "Invalid ip address";
    default:
        return "Unknown error";
    }
}

} // namespace error
} // namespace conet