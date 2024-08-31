#pragma once

#include <cstdint>

#include <CS2/Classes/EntitySystem/CEntityHandle.h>
#include "GlowSceneObjectPointer.h"

struct GlowSceneObjectsState {
    using SizeType = std::uint16_t;
    static constexpr auto kInvalidIndex = static_cast<SizeType>(-1);

    cs2::CEntityHandle* entityHandles{nullptr};
    GlowSceneObjectPointer* glowSceneObjects{nullptr};
    SizeType size{0};
    SizeType capacity{0};
};
