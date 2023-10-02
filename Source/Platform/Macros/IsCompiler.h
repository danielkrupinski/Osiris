#pragma once

#if defined(__clang__)
    #define CLANG
#elif defined(__GNUC__)
    #define GCC
#elif defined(_MSC_VER)
    #define MSVC
#endif
