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

    void applyGlowToEntity(EntityTypeInfo entityTypeInfo, auto&& modelEntity) noexcept
    {
        if (!hookContext.config().template getVariable<outline_glow_vars::Enabled>())
            return;

        if (modelEntity.glowProperty().isGlowing().valueOr(false))
            return;

        if (entityTypeInfo.is<cs2::C_CSPlayerPawn>())
            applyGlow(entityTypeInfo, hookContext.template make<PlayerOutlineGlow>(), modelEntity.template as<PlayerPawn>());
        else if (entityTypeInfo.is<cs2::CBaseAnimGraph>())
            applyGlow(entityTypeInfo, hookContext.template make<DefuseKitOutlineGlow>(), modelEntity.baseEntity());
        else if (entityTypeInfo.is<cs2::CPlantedC4>())
            applyGlow(entityTypeInfo, hookContext.template make<TickingBombOutlineGlow>(), modelEntity.template as<PlantedC4>());
        else if (entityTypeInfo.is<cs2::C_C4>())
            applyGlow(entityTypeInfo, hookContext.template make<DroppedBombOutlineGlow>(), modelEntity.baseEntity());
        else if (entityTypeInfo.is<cs2::C_Hostage>())
            applyGlow(entityTypeInfo, hookContext.template make<HostageOutlineGlow>(), modelEntity.baseEntity());
        else if (entityTypeInfo.isGrenadeProjectile())
            applyGlow(entityTypeInfo, hookContext.template make<GrenadeProjectileOutlineGlow>(), modelEntity.baseEntity());
        else if (entityTypeInfo.isWeapon())
            applyGlow(entityTypeInfo, hookContext.template make<WeaponOutlineGlow>(), modelEntity.baseEntity());
    }

    void onUnload() const noexcept
    {
        hookContext.template make<GlowSceneObjects>().clearObjects();
    }

private:
    static void applyGlow(EntityTypeInfo entityTypeInfo, auto&& glow, auto&& entity) noexcept
    {
        if (glow.shouldApplyGlow(entityTypeInfo, entity))
            entity.baseEntity().applyGlowRecursively(getGlowColor(entityTypeInfo, glow, entity), getGlowRange(glow));
    }

    [[nodiscard]] static cs2::Color getGlowColor(EntityTypeInfo entityTypeInfo, auto&& glow, auto&& entity)
    {
        using namespace outline_glow_params;
        if (const auto hue = glow.getGlowHue(entityTypeInfo, entity); hue.hasValue())
            return color::HSBtoRGB(hue.value(), kSaturation, kBrightness).setAlpha(getGlowColorAlpha(glow, entity));
        return kFallbackColor;
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
