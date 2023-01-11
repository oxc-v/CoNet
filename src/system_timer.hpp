/**
 * 使用chrono::system_clock作定时器的时钟类型 
 */

#pragma once

#include "basic_timer.hpp"

namespace conet {
namespace system_timer {

using SystemTimer = basic_timer::BasicTimer<std::chrono::system_clock>;

} // system_timer
} // conet
