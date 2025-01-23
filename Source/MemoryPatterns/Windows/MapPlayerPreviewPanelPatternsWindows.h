#pragma once

#include <MemoryPatterns/PatternTypes/MapPlayerPreviewPanelPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct MapPlayerPreviewPanelPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<OffsetToPlayerPreviewPanelEntities, CodePattern{"F3 0F 11 ? ? ? ? ? 44 39 ? ? 0F"}.add(11).read()>();
    }
};
