#pragma once

#include <chrono>  // system_clock, milliseconds, microseconds, nanoseconds

namespace gravel {

namespace chrono = std::chrono;

using Milliseconds = chrono::milliseconds;
using Microseconds = chrono::microseconds;
using Nanoseconds = chrono::nanoseconds;

using Clock = chrono::system_clock;

using TimePoint = Clock::time_point;
using Duration = Clock::duration;
using TimeRep = Clock::rep;

}  // namespace gravel
