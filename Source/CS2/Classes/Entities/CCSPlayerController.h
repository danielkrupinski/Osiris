#pragma once

#include <cstdint>

#include <CS2/Classes/EntitySystem/CEntityHandle.h>
#include <Platform/Macros/PlatformSpecific.h>

#include "C_BaseEntity.h"

namespace cs2
{

enum class PlayerColorIndex : std::int32_t {
    Blue = 0,
    Green = 1,
    Yellow = 2,
    Orange = 3,
    Purple = 4
};

constexpr auto kPlayerColorIndexCount{5};

struct CCSPlayerController : C_BaseEntity {
    static constexpr auto kMangledTypeName{WIN64_LINUX(".?AVCCSPlayerController@@", "19CCSPlayerController")};

    using m_hPawn = CEntityHandle;
    using m_iCompTeammateColor = PlayerColorIndex;
};

}
