#pragma once

#include <MemoryPatterns/PatternTypes/PanoramaImagePanelPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct PanoramaImagePanelPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<SetImageFunctionPointer, CodePattern{"CF E8 ? ? ? ? 41 0F B6 C5"}.add(2).abs()>()
            .template addPattern<ImagePanelConstructorPointer, CodePattern{"? ? ? ? 48 8B C8 EB 03 49 8B ? 48 89"}.abs()>()
            .template addPattern<ImagePanelClassSize, CodePattern{"90 ? ? ? ? B9 ? ? ? ? E8 ? ? ? ? 48 85 C0 74 ? 48 8B"}.add(6).read()>()
            .template addPattern<ImagePropertiesOffset, CodePattern{"EB 08 41 C7 47 ? 00 00 80 BF ? 8D ? ?"}.add(13).read()>()
            .template addPattern<OffsetToImagePath, CodePattern{"B6 DA 48 81 C1 ? ? ? ?"}.add(5).read()>();
    }
};
