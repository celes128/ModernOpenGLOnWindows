#pragma once
#include <cstdint>

namespace dbg {
    void log_update(uint64_t t);
    void log_render();

    void printf(const char* fmt, ...);
    void wprintf(const wchar_t* fmt, ...);
};

