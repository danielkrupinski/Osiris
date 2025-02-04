#pragma once

#include <MemoryPatterns/PatternTypes/PanoramaLabelPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct PanoramaLabelPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<LabelPanelConstructorPointer, CodePattern{"E8 ? ? ? ? 48 8B D8 33 FF EB"}.add(1).abs()>()
            .template addPattern<SetLabelTextFunctionPointer, CodePattern{"41 B1 01 41 B8 ? ? ? ? E9 ? ? ? ?"}.add(10).abs()>()
            .template addPattern<LabelPanelObjectSize, CodePattern{"FF 90 ? ? ? ? B9 ? ? ? ? E8 ? ? ? ? 48 85 C0 74 ? 45 33 C0 49 8B D4"}.add(7).read()>();
    }
};
