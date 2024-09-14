#pragma once

#include <utility>
#include "OutlineGlowContext.h"

template <typename Context>
class OutlineGlow {
public:
    template <typename... Args>
    OutlineGlow(Args&&... args) noexcept
        : context{std::forward<Args>(args)...}
    {
    }

    void applyGlowToEntity(EntityTypeInfo entityTypeInfo, cs2::CEntityInstance& entity) noexcept
    {
        if (!context.state().enabled)
            return;

        if (entityTypeInfo.typeIndex == utils::typeIndex<cs2::C_CSPlayerPawn, KnownEntityTypes>())
            context.applyGlowToPlayer(entity);
        else if (entityTypeInfo.isWeapon())
            context.applyGlowToWeapon(entity);
    }

private:
    Context context;
};

template <typename HookContext>
OutlineGlow(HookContext&) -> OutlineGlow<OutlineGlowContext<HookContext>>;
