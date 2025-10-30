#pragma once

#include <CS2/Classes/Entities/WeaponEntities.h>
#include <MemoryPatterns/PatternTypes/C4PatternTypes.h>
#include "BaseWeapon.h"

template <typename HookContext>
class C4 {
public:
    C4(HookContext& hookContext, cs2::C_C4* c4) noexcept
        : hookContext{hookContext}
        , c4{c4}
    {
    }

    [[nodiscard]] decltype(auto) baseWeapon() const noexcept
    {
        return hookContext.template make<BaseWeapon>(c4);
    }

    [[nodiscard]] explicit operator bool() const noexcept
    {
        return c4 != nullptr;
    }

    [[nodiscard]] decltype(auto) isBeingPlanted() const noexcept
    {
        return hookContext.patternSearchResults().template get<OffsetToIsBeingPlanted>().of(c4);
    }

    [[nodiscard]] decltype(auto) armingEndTime() const noexcept
    {
        return hookContext.patternSearchResults().template get<OffsetToArmingEndTime>().of(c4).toOptional();
    }

    [[nodiscard]] decltype(auto) nearestBombsiteIndex() const
    {
        return baseWeapon().baseEntity().absOrigin().andThen(toNearestBombsiteIndex());
    }

    [[nodiscard]] decltype(auto) timeToArmingEnd() const
    {
        return armingEndTime() - hookContext.globalVars().curtime();
    }

    using RawType = cs2::C_C4;

private:
    [[nodiscard]] auto toNearestBombsiteIndex() const noexcept
    {
        return [this](const auto& position) {
            return hookContext.playerResource().nearestBombsiteIndex(position);
        };
    }

    HookContext& hookContext;
    cs2::C_C4* c4;
};
