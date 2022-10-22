#pragma once

#if defined(_WIN32)

#define IS_WIN32() true

#elif defined(__linux__)

#define IS_WIN32() false

#else

#define IS_WIN32() false

#endif
