#pragma once

#include <cstdint>

#include <CS2/Classes/ClientModeCSNormal.h>
#include <GameClasses/OffsetTypes/ClientModeOffset.h>

using ZoomedSniperEffectWeightOffset = ClientModeOffset<cs2::ClientModeCSNormal::zoomedSniperEffectWeight, std::int32_t>;
using ZoomedMovingSniperEffectWeightOffset = ClientModeOffset<cs2::ClientModeCSNormal::zoomedMovingSniperEffectWeight, std::int32_t>;

struct ClientModePatterns {
    [[nodiscard]] static ZoomedSniperEffectWeightOffset zoomedSniperEffectWeightOffset() noexcept;
    [[nodiscard]] static ZoomedMovingSniperEffectWeightOffset zoomedMovingSniperEffectWeightOffset() noexcept;
};
