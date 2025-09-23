#pragma once

#include <utility>

#include <CS2/Classes/CPlantedC4.h>
#include <CS2/Classes/Entities/CBaseAnimGraph.h>
#include <CS2/Classes/Entities/C_CSPlayerPawn.h>
#include <CS2/Classes/Entities/C_Hostage.h>
#include <CS2/Classes/Entities/WeaponEntities.h>
#include <GameClient/Entities/PlantedC4.h>
#include <GameClient/Entities/PlayerPawn.h>
#include <GameClient/Entities/EntityClassifier.h>

#include "DefuseKitOutlineGlow/DefuseKitOutlineGlow.h"
#include "DroppedBombOutlineGlow/DroppedBombOutlineGlow.h"
#include "GrenadeProjectileOutlineGlow/GrenadeProjectileOutlineGlow.h"
#include "HostageOutlineGlow/HostageOutlineGlow.h"
#include "PlayerOutlineGlow/PlayerOutlineGlow.h"
#include "TickingBombOutlineGlow/TickingBombOutlineGlow.h"
#include "WeaponOutlineGlow/WeaponOutlineGlow.h"

template <typename HookContext>
class OutlineGlow {
public:
    explicit OutlineGlow(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    void applyGlowToEntity(EntityTypeInfo entityTypeInfo, auto&& modelEntity)
    {
        if (!hookContext.config().template getVariable<outline_glow_vars::Enabled>())
            return;

        if (modelEntity.glowProperty().isGlowing().valueOr(false))
            return;

        if (entityTypeInfo.is<cs2::C_CSPlayerPawn>())
            applyGlow(entityTypeInfo, PlayerOutlineGlow{hookContext}, modelEntity.template as<PlayerPawn>());
        else if (entityTypeInfo.is<cs2::CBaseAnimGraph>())
            applyGlow(entityTypeInfo, DefuseKitOutlineGlow{hookContext}, modelEntity.baseEntity());
        else if (entityTypeInfo.is<cs2::CPlantedC4>())
            applyGlow(entityTypeInfo, TickingBombOutlineGlow{hookContext}, modelEntity.template as<PlantedC4>());
        else if (entityTypeInfo.is<cs2::C_C4>())
            applyGlow(entityTypeInfo, DroppedBombOutlineGlow{hookContext}, modelEntity.baseEntity());
        else if (entityTypeInfo.is<cs2::C_Hostage>())
            applyGlow(entityTypeInfo, HostageOutlineGlow{hookContext}, modelEntity.baseEntity());
        else if (entityTypeInfo.isGrenadeProjectile())
            applyGlow(entityTypeInfo, GrenadeProjectileOutlineGlow{hookContext}, modelEntity.baseEntity());
        else if (entityTypeInfo.isWeapon())
            applyGlow(entityTypeInfo, WeaponOutlineGlow{hookContext}, modelEntity.baseEntity());
    }

    void onUnload() const noexcept
    {
        hookContext.template make<GlowSceneObjects>().clearObjects();
    }

private:
    static void applyGlow(EntityTypeInfo entityTypeInfo, auto&& glow, auto&& entity)
    {
        if (glow.shouldApplyGlow(entityTypeInfo, entity))
            entity.baseEntity().applyGlowRecursively(getGlowColor(entityTypeInfo, glow, entity), getGlowRange(glow));
    }

    [[nodiscard]] static cs2::Color getGlowColor(EntityTypeInfo entityTypeInfo, auto&& glow, auto&& entity)
    {
        using namespace outline_glow_params;
        const auto alpha = getGlowColorAlpha(glow, entity);
        if (const auto hue = glow.getGlowHue(entityTypeInfo, entity); hue.hasValue())
            return color::HSBtoRGB(hue.value(), kSaturation, kBrightness).setAlpha(alpha);
        return kFallbackColor.setAlpha(alpha);
    }

    [[nodiscard]] static std::uint8_t getGlowColorAlpha(auto&& glow, auto&& entity) noexcept
    {
        if constexpr (requires { { glow.getGlowColorAlpha(entity) } -> std::same_as<std::uint8_t>; })
            return glow.getGlowColorAlpha(entity);
        else
            return outline_glow_params::kGlowAlpha;
    }

    [[nodiscard]] static int getGlowRange(auto&& glow) noexcept
    {
        if constexpr (requires { { glow.getGlowRange() } -> std::same_as<int>; })
            return glow.getGlowRange();
        else
            return 0;
    }

    HookContext& hookContext;
};
