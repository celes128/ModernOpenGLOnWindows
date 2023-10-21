#pragma once
#include <chrono>
#include "../miny/types.h"

// https://stackoverflow.com/questions/21856025/getting-an-accurate-execution-time-in-c-micro-seconds
// Get time stamp in microseconds.
inline u64 get_time_usec()
{
    u64 us = std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch())
        .count();

    return us;
}