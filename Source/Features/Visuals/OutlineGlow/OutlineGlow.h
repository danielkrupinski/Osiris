#pragma once

#include <utility>

#include <CS2/Classes/CPlantedC4.h>
#include <CS2/Classes/Entities/CBaseAnimGraph.h>
#include <CS2/Classes/Entities/C_CSPlayerPawn.h>
#include <CS2/Classes/Entities/C_Hostage.h>
#include <CS2/Classes/Entities/WeaponEntities.h>
#include <GameClient/Entities/BaseModelEntity.h>
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

    [[nodiscard]] auto applyGlow() const noexcept
    {
        return [this](auto&& glow, auto&& entity, EntityTypeInfo entityTypeInfo) {
            if (!GET_CONFIG_VAR(outline_glow_vars::Enabled) || !glow.enabled())
                return;

            if (!entityTypeInfo.isModelEntity() || entity.baseEntity().template as<BaseModelEntity>().glowProperty().isGlowing().valueOr(false))
                return;

            if (shouldApplyGlow(glow, entityTypeInfo, entity))
                entity.baseEntity().applyGlowRecursively(getGlowColor(glow, entity, entityTypeInfo), getGlowRange(glow));
        };
    }

    void onUnload() const noexcept
    {
        hookContext.template make<GlowSceneObjects>().clearObjects();
    }

private:
    [[nodiscard]] static bool shouldApplyGlow(auto&& glow, EntityTypeInfo entityTypeInfo, auto&& entity)
    {
        if constexpr (requires { { glow.shouldApplyGlow(entityTypeInfo, entity) } -> std::same_as<bool>; })
            return glow.shouldApplyGlow(entityTypeInfo, entity);
        else
            return true;
    }

    template <typename Hue>
    [[nodiscard]] static cs2::Color colorFromHue(Optional<Hue> hue) noexcept
    {
        using namespace outline_glow_params;
        if (hue.hasValue())
            return color::HSBtoRGB(hue.value(), kSaturation, kBrightness);
        return kFallbackColor;
    }

    [[nodiscard]] static cs2::Color colorFromHue(auto hue) noexcept
    {
        using namespace outline_glow_params;
        return color::HSBtoRGB(hue, kSaturation, kBrightness);
    }

    [[nodiscard]] static auto getGlowHue(auto&& glow, auto&& entity, EntityTypeInfo entityTypeInfo)
    {
        if constexpr (requires { { glow.hue(entityTypeInfo, entity) }; })
            return glow.hue(entityTypeInfo, entity);
        else
            return glow.hue();
    }

    [[nodiscard]] static auto getGlowColor(auto&& glow, auto&& entity, EntityTypeInfo entityTypeInfo)
    {
        return colorFromHue(getGlowHue(glow, entity, entityTypeInfo)).setAlpha(getGlowColorAlpha(glow, entity));
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
