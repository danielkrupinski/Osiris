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
        return hookContext.clientPatternSearchResults().template get<OffsetToIsBeingPlanted>().of(c4);
    }

    using RawType = cs2::C_C4;

private:
    HookContext& hookContext;
    cs2::C_C4* c4;
};
