#pragma once

#include <cstdint>
#include <Platform/CallingConventions.h>
#include <Utils/TypeHint.h>

namespace csgo::pod { struct EconItem; }

struct EconItemFunctions {
    std::uintptr_t setDynamicAttributeValue;
    std::uintptr_t removeDynamicAttribute;
    TypeHint<std::uintptr_t, void(THISCALL_CONV*)(csgo::pod::EconItem* thisptr, const char* name)> setCustomName;
};
