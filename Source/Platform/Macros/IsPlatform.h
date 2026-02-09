#pragma once

#if defined(_WIN64) && defined(__linux__)
    #error "Conflicting platform macros defined"
#endif

#if defined(_WIN64)
    #define IS_WIN64() 1
    #define IS_LINUX() 0
#elif defined(__linux__)
    #define IS_WIN64() 0
    #define IS_LINUX() 1
#else
    #define IS_WIN64() 0
    #define IS_LINUX() 0
#endif
