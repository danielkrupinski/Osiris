#pragma once

#include <cstdint>

#include "IsPlatform.h"
#include <Utils/ReturnAddress.h>

#if IS_WIN32()

#include <intrin.h>

#define RETURN_ADDRESS() ReturnAddress{ std::uintptr_t(_ReturnAddress()) }
#define FRAME_ADDRESS() (std::uintptr_t(_AddressOfReturnAddress()) - sizeof(std::uintptr_t))

#elif IS_LINUX()

#define RETURN_ADDRESS() ReturnAddress{ std::uintptr_t(__builtin_return_address(0)) }
#define FRAME_ADDRESS() std::uintptr_t(__builtin_frame_address(0))

#else

#define RETURN_ADDRESS()
#define FRAME_ADDRESS()

#endif
