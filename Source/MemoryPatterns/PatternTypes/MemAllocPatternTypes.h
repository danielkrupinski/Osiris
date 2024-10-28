#pragma once

#include <cstdint>
#include <CS2/Classes/IMemAlloc.h>
#include <Utils/FieldOffset.h>
#include <Utils/StrongTypeAlias.h>

STRONG_TYPE_ALIAS(OffsetAllocVirtualMethod, FieldOffset<const void, cs2::IMemAlloc::Alloc*, std::int8_t>);
