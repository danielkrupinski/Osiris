#pragma once

#include <MemoryPatterns/PatternTypes/WeaponPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct WeaponPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<OffsetToClipAmmo, CodePattern{"? ? ? ? 85 C0 7E ? 85 C0 ? 8B"}.read()>()
            .template addPattern<OffsetToWeaponSceneObjectUpdaterHandle, CodePattern{"48 89 83 ? ? ? ? BE ? ? ? ? 48 89 DF"}.add(3).read()>()
            .template addPattern<PointerToGetInaccuracyFunction, CodePattern{"55 48 89 E5 41 57 41 56 49 89 ? 41 55 49 89 ? 41 54 53 48 89 FB 48 83 EC ? E8"}>()
            .template addPattern<PointerToGetSpreadFunction, CodePattern{"55 48 89 E5 48 83 EC ? 48 63"}>();
    }
};
