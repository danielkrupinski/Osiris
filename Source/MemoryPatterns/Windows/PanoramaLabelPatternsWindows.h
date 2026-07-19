#pragma once

#include <MemoryPatterns/PatternTypes/PanoramaLabelPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct PanoramaLabelPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<LabelPanelConstructorPointer, CodePattern{"C8 E8 ? ? ? ? 48 8B F0 48 8B 57"}.add(2).abs()>()
            .template addPattern<SetLabelTextFunctionPointer, CodePattern{"C6 44 24 ? ? 41 B8 ? ? ? ? E8 ? ? ? ? 48 83"}.add(12).abs()>()
            .template addPattern<LabelPanelObjectSize, CodePattern{"66 0F 6E F6 B9 ? ? ? ?"}.add(5).read()>();
    }
};
