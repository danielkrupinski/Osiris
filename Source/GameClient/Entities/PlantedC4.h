#pragma once

#include <utility>

#include <CS2/Classes/CPlantedC4.h>
#include <CS2/Constants/BombsiteIndex.h>
#include <CS2/Constants/EntityHandle.h>
#include <CS2/Constants/IconURLs.h>
#include <MemoryPatterns/PatternTypes/PlantedC4PatternTypes.h>

#include "BaseEntity.h"

template <typename HookContext>
class PlantedC4 {
public:
    using RawType = cs2::CPlantedC4;

    PlantedC4(HookContext& hookContext, cs2::CPlantedC4* plantedC4) noexcept
        : hookContext{hookContext}
        , plantedC4{plantedC4}
    {
    }

    [[nodiscard]] decltype(auto) baseEntity() const noexcept
    {
        return hookContext.template make<BaseEntity>(plantedC4);
    }

    [[nodiscard]] auto getTimeToExplosion() const noexcept
    {
        return blowTime() - curtime();
    }

    [[nodiscard]] auto isTicking() const noexcept
    {
        return hookContext.patternSearchResults().template get<BombTickingOffset>().of(plantedC4).toOptional();
    }

    [[nodiscard]] bool isBeingDefused() const noexcept
    {
        return defuser().valueOr(cs2::INVALID_EHANDLE_INDEX) != cs2::INVALID_EHANDLE_INDEX;
    }

    [[nodiscard]] auto canBeDefused() const noexcept
    {
        return defuseEndTime().lessThan(blowTime());
    }

    [[nodiscard]] auto getTimeToDefuseEnd() const noexcept
    {
        return defuseEndTime() - curtime();
    }

    [[nodiscard]] auto bombsiteIndex() const noexcept
    {
        return baseEntity().absOrigin().andThen(toNearestBombsiteIndex());
    }

private:
    [[nodiscard]] auto toNearestBombsiteIndex() const noexcept
    {
        return [this](const auto& position) {
            return hookContext.playerResource().nearestBombsiteIndex(position);
        };
    }

    [[nodiscard]] auto blowTime() const noexcept
    {
        return hookContext.patternSearchResults().template get<BombBlowTimeOffset>().of(plantedC4).toOptional();
    }

    [[nodiscard]] auto defuser() const noexcept
    {
        return hookContext.patternSearchResults().template get<BombDefuserOffset>().of(plantedC4);
    }

    [[nodiscard]] auto defuseEndTime() const noexcept
    {
        return hookContext.patternSearchResults().template get<BombDefuseEndTimeOffset>().of(plantedC4).toOptional();
    }

    [[nodiscard]] decltype(auto) curtime() const noexcept
    {
        return hookContext.globalVars().curtime();
    }

    HookContext& hookContext;
    cs2::CPlantedC4* plantedC4;
};
