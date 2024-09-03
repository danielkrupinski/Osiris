#pragma once

#include <cstdint>

#include "GlowSceneObjectPointer.h"

struct GlowSceneObjectsState {
    using SizeType = std::uint16_t;
    static constexpr auto kInvalidIndex = static_cast<SizeType>(-1);

    GlowSceneObjectPointer* glowSceneObjects{nullptr};
    SizeType size{0};
    SizeType capacity{0};
};
