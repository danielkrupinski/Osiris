#pragma once

#include <Platform/CallingConventions.h>
#include <Utils/TypeHint.h>

namespace csgo::pod { struct EconItem; }
namespace csgo::pod { struct EconItemView; }

struct EconItemViewFunctions {
    TypeHint<std::uintptr_t, void (THISCALL_CONV*)(csgo::pod::EconItemView* thisptr)> clearInventoryImageRGBA;
    TypeHint<std::uintptr_t, csgo::pod::EconItem* (THISCALL_CONV*)(csgo::pod::EconItemView* thisptr)> getSOCData;
};
