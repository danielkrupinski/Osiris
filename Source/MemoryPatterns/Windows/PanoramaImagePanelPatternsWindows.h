#pragma once

#include <MemoryPatterns/PatternTypes/PanoramaImagePanelPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct PanoramaImagePanelPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<SetImageFunctionPointer, CodePattern{"48 89 5C 24 18 57 48 83 EC 20 48 8B FA 48 8B D9 48 85 D2 0F 84 ? ? ? ? 80 3A 00 0F 84 ? ? ? ? 48 81 C1 98 00 00 00 FF 15 ? ? ? ? 84 C0 0F 85"}>()
            .template addPattern<ImagePanelConstructorPointer, CodePattern{"? ? ? ? 48 8B C8 EB 03 49 8B ? 48 89"}.abs()>()
            .template addPattern<ImagePanelClassSize, CodePattern{"FF 15 ? ? ? ? B9 ? ? ? ? E8 ? ? ? ? 48 85 C0 74 ? 48 8B ? ? ? ? ? 4C"}.add(7).read()>()
            .template addPattern<ImagePropertiesOffset, CodePattern{"EB 08 41 C7 47 ? 00 00 80 BF ? 8D ? ?"}.add(13).read()>()
            .template addPattern<OffsetToImagePath, CodePattern{"B6 DA 48 81 C1 ? ? ? ?"}.add(5).read()>();
    }
};
