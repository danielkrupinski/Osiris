#pragma once

#include <CS2/Classes/CCSWeaponBaseVData.h>
#include <MemoryPatterns/PatternTypes/WeaponVDataPatternTypes.h>

template <typename HookContext>
class WeaponVData {
public:
    WeaponVData(HookContext& hookContext, cs2::CCSWeaponBaseVData* vData) noexcept
        : hookContext{hookContext}
        , vData{vData}
    {
    }

    [[nodiscard]] auto name() const
    {
        return hookContext.patternSearchResults().template get<OffsetToWeaponName>().of(vData);
    }

    [[nodiscard]] auto spread() const
    {
        return hookContext.patternSearchResults().template get<OffsetToWeaponSpread>().of(vData);
    }

private:
    HookContext& hookContext;
    cs2::CCSWeaponBaseVData* vData;
};
