#pragma once

#include <cstdint>
#include <Platform/Macros/CallingConventions.h>
#include <Utils/TypeHint.h>

namespace csgo { struct EconItemPOD; }

struct EconItemFunctions {
    std::uintptr_t setDynamicAttributeValue;
    std::uintptr_t removeDynamicAttribute;
    TypeHint<std::uintptr_t, void(THISCALL_CONV*)(csgo::EconItemPOD* thisptr, const char* name)> setCustomName;
};
