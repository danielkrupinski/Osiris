#pragma once

#include <GameClasses/OffsetTypes/ClientModeOffset.h>
#include <MemoryPatterns/ClientModePatterns.h>

struct ClientModeImpl {
    explicit ClientModeImpl(const ClientModePatterns& clientModePatterns) noexcept
        : zoomedSniperEffectWeight{clientModePatterns.zoomedSniperEffectWeightOffset()}
        , zoomedMovingSniperEffectWeight{clientModePatterns.zoomedMovingSniperEffectWeightOffset()}
    {
    }

    [[nodiscard]] static const ClientModeImpl& instance() noexcept;

    ZoomedSniperEffectWeightOffset zoomedSniperEffectWeight;
    ZoomedMovingSniperEffectWeightOffset zoomedMovingSniperEffectWeight;
};
