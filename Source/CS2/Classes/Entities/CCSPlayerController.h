#pragma once

#include <cstdint>

#include <CS2/Classes/EntitySystem/CEntityHandle.h>
#include <Platform/Macros/PlatformSpecific.h>

#include "C_BaseEntity.h"

namespace cs2
{

struct CCSPlayerController : C_BaseEntity {
    static constexpr auto kMangledTypeName{WIN32_LINUX(".?AVCCSPlayerController@@", "19CCSPlayerController")};

    using m_hPawn = CEntityHandle;
    using m_iCompTeammateColor = std::int32_t;
};

}
