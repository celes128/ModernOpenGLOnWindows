#include "dbg.h"
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <iostream>
#include <strsafe.h>
#include <cstdarg>


namespace dbg {
    const int DBG_MAX_N = 8;

    void log_update(uint64_t t)
    {
        static int n = 0;

        if (n < DBG_MAX_N) {
            dbg::wprintf(L"---------------------\n", t);
            dbg::wprintf(L"Update - t = %llu\n", t);
            n++;
        }
    }

    void log_render()
    {
        static int n = 0;

        if (n < DBG_MAX_N) {
            dbg::wprintf(L"Render\n");
            n++;
        }
    }

    void printf(const char* fmt, ...)
    {
        static const int N = 512;
        static char buf[N] = { 0 };
        
        va_list args;
        
        va_start(args, fmt);
        _vsnprintf_s(buf, N, fmt, args);
        va_end(args);

        OutputDebugStringA(buf);
    }
    
    void wprintf(const wchar_t* fmt, ...)
    {
        static const int N = 512;
        static wchar_t buf[N] = { 0 };

        va_list args;

        va_start(args, fmt);
        _vsnwprintf_s(buf, N, fmt, args);
        va_end(args);

        OutputDebugStringW(buf);
    }
}