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

        if (entityTypeInfo.is<cs2::C_CSPlayerPawn>())
            context.applyGlowToPlayer(entity);
        else if (entityTypeInfo.is<cs2::CBaseAnimGraph>())
            context.applyGlowToDefuseKit(entity);
        else if (entityTypeInfo.is<cs2::CPlantedC4>())
            context.applyGlowToPlantedBomb(entity);
        else if (entityTypeInfo.is<cs2::C_C4>())
            context.applyGlowToBomb(entity);
        else if (entityTypeInfo.is<cs2::C_Hostage>())
            context.applyGlowToHostage(entity);
        else if (entityTypeInfo.isGrenadeProjectile())
            context.applyGlowToGrenadeProjectile(entityTypeInfo, entity);
        else if (entityTypeInfo.isWeapon())
            context.applyGlowToWeapon(entityTypeInfo, entity);
    }

private:
    Context context;
};

template <typename HookContext>
OutlineGlow(HookContext&) -> OutlineGlow<OutlineGlowContext<HookContext>>;
