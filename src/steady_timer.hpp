/**
 * 使用chrono::steady_clock作定时器的时钟类型 
 */

#pragma once

#include "basic_timer.hpp"

namespace conet {
namespace steady_timer {

using SteadyTimer = basic_timer::BasicTimer<std::chrono::steady_clock>;

} // steady_timer
} // conet
