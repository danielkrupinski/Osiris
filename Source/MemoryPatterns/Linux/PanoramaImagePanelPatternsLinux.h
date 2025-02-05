#pragma once

#include <MemoryPatterns/PatternTypes/PanoramaImagePanelPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct PanoramaImagePanelPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<SetImageFunctionPointer, CodePattern{"55 48 89 E5 41 57 49 89 F7 41 56 49 89 D6 41 55 41 54 49 89 CC"}>()
            .template addPattern<ImagePanelConstructorPointer, CodePattern{"D2 48 89 C7 48 89 85 ? ? ? ? E8 ? ? ? ? 4C 8B 85 ? ? ? ? 4C"}.add(12).abs()>()
            .template addPattern<ImagePanelClassSize, CodePattern{"BF ? ? ? ? 4C 89 85 ? ? ? ? E8 ? ? ? ? 48 8B B5"}.add(1).read()>()
            .template addPattern<ImagePropertiesOffset, CodePattern{"4C 89 E7 49 8D 4C 24 ?"}.add(7).read()>()
            .template addPattern<OffsetToImagePath, CodePattern{"48 81 C7 ? ? ? ? 53 89 F3 48"}.add(3).read()>();
    }
};
