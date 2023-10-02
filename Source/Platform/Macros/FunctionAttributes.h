#pragma once

#include "IsCompiler.h"

#if defined(GCC)
    #define NOINLINE gnu::noinline
#elif defined(CLANG)
    #define NOINLINE clang::noinline
#elif defined(MSVC)
    #define NOINLINE msvc::noinline
#else
    #define NOINLINE
#endif
