#pragma once

#include <GameClasses/OffsetTypes/ClientModeOffset.h>
#include <MemoryPatterns/ClientModePatterns.h>

struct ClientModeImpl {
    [[nodiscard]] static const ClientModeImpl& instance() noexcept;

    ZoomedSniperEffectWeightOffset zoomedSniperEffectWeight{ ClientModePatterns::zoomedSniperEffectWeightOffset() };
    ZoomedMovingSniperEffectWeightOffset zoomedMovingSniperEffectWeight{ ClientModePatterns::zoomedMovingSniperEffectWeightOffset() };
};
