#pragma once

#include <cstdint>
#include <CS2/Classes/IMemAlloc.h>
#include <Utils/FieldOffset.h>

#include "StrongType.h"

STRONG_TYPE(OffsetAllocVirtualMethod, FieldOffset<const void, cs2::IMemAlloc::Alloc*, std::int8_t>);
