#pragma once

#include <Platform/Macros/CallingConventions.h>
#include <Utils/TypeHint.h>

namespace csgo { struct EconItemPOD; }
namespace csgo { struct EconItemViewPOD; }

struct EconItemViewFunctions {
    TypeHint<std::uintptr_t, void (THISCALL_CONV*)(csgo::EconItemViewPOD* thisptr)> clearInventoryImageRGBA;
    TypeHint<std::uintptr_t, csgo::EconItemPOD* (THISCALL_CONV*)(csgo::EconItemViewPOD* thisptr)> getSOCData;
};
