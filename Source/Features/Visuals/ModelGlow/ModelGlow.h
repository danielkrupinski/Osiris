#pragma once

#include <CS2/Classes/CPlantedC4.h>
#include <CS2/Classes/Entities/CBaseAnimGraph.h>
#include <CS2/Classes/Entities/C_CSPlayerPawn.h>
#include <CS2/Classes/Entities/WeaponEntities.h>
#include <GameClient/Entities/BaseWeapon.h>
#include <GameClient/Entities/C4.h>
#include <GameClient/Entities/PlantedC4.h>
#include <GameClient/Entities/PlayerPawn.h>
#include <GameClient/Entities/EntityClassifier.h>

#include "DefuseKitModelGlow/DefuseKitModelGlow.h"
#include "DroppedBombModelGlow/DroppedBombModelGlow.h"
#include "GrenadeProjectileModelGlow/GrenadeProjectileModelGlow.h"
#include "ModelGlowState.h"
#include "PlayerModelGlow/PlayerModelGlow.h"
#include "TickingBombModelGlow/TickingBombModelGlow.h"
#include "WeaponModelGlow/WeaponModelGlow.h"

template <typename HookContext>
class ModelGlow {
public:
    ModelGlow(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    [[nodiscard]] auto updateInMainThread() const noexcept
    {
        return [this](auto&& glow, auto&& entity, EntityTypeInfo entityTypeInfo) {
            if (!hookContext.config().template getVariable<model_glow_vars::Enabled>() && !state().modelGlowDisabling)
                return;
            if (!glow.enabled() && !glow.disablingFlag())
                return;

            if (hookContext.config().template getVariable<model_glow_vars::Enabled>() && glow.enabled() && shouldApplyGlow(glow, entity))
                applyGlowInMainThread(glow, entityTypeInfo, entity);
            else
                removeGlowInMainThread(glow, entity);
        };
    }

    [[nodiscard]] auto updateInSceneObjectUpdater() const noexcept
    {
        return [this](auto&& glow, auto&& entity, EntityTypeInfo entityTypeInfo) {
            if (hookContext.config().template getVariable<model_glow_vars::Enabled>() && glow.enabled() && shouldApplyGlow(glow, entity))
                entity.baseEntity().applySpawnProtectionEffectRecursively(getGlowHue(glow, entity, entityTypeInfo));
        };
    }

    [[nodiscard]] auto onUnload() const noexcept
    {
        return [this](auto&& glow, auto&& entity) {
            assert(state().modelGlowDisabling == false && "Should be already disabled");
            assert(glow.disablingFlag() == false && "Should be already disabled");

            if (!hookContext.config().template getVariable<model_glow_vars::Enabled>())
                return;

            if (glow.enabled()) {
                if constexpr (isUsingSceneObjectUpdaterHook<decltype(glow)>()) {
                    if (hasSceneObjectUpdaterHooked(glow, entity))
                        entity.setSceneObjectUpdater(glow.originalSceneObjectUpdater());
                } else {
                    entity.baseEntity().removeSpawnProtectionEffectRecursively();
                }
            }
        };
    }

    void onEntityListTraversed() const noexcept
    {
        state().modelGlowDisabling = false;
        hookContext.template make<PlayerModelGlow>().disablingFlag() = false;
        hookContext.template make<WeaponModelGlow>().disablingFlag() = false;
        hookContext.template make<DroppedBombModelGlow>().disablingFlag() = false;
        hookContext.template make<TickingBombModelGlow>().disablingFlag() = false;
        hookContext.template make<DefuseKitModelGlow>().disablingFlag() = false;
        hookContext.template make<GrenadeProjectileModelGlow>().disablingFlag() = false;
    }

private:
    [[nodiscard]] static bool shouldApplyGlow(auto&& glow, auto&& entity)
    {
        if constexpr (requires { { glow.shouldApplyGlow(entity) } -> std::same_as<bool>; })
            return glow.shouldApplyGlow(entity);
        else
            return true;
    }

    template <typename Glow>
    [[nodiscard]] static constexpr bool isUsingSceneObjectUpdaterHook() noexcept
    {
        return requires (Glow glow) {
            { glow.originalSceneObjectUpdater() };
            { glow.replacementSceneObjectUpdater() };
        };
    }

    void applyGlowInMainThread(auto&& glow, [[maybe_unused]] EntityTypeInfo entityTypeInfo, auto&& entity) const
    {
        if constexpr (isUsingSceneObjectUpdaterHook<decltype(glow)>()) {
            if (!hasSceneObjectUpdaterHooked(glow, entity)) {
                storeOriginalSceneObjectUpdater(glow, entity);
                hookSceneObjectUpdater(glow, entity);
            }
        } else {
            entity.baseEntity().applySpawnProtectionEffectRecursively(getGlowHue(glow, entity, entityTypeInfo));
        }
    }

    void removeGlowInMainThread(auto&& glow, auto&& entity) const
    {
        if constexpr (isUsingSceneObjectUpdaterHook<decltype(glow)>()) {
            if (hasSceneObjectUpdaterHooked(glow, entity))
                entity.setSceneObjectUpdater(glow.originalSceneObjectUpdater());
        } else {
            entity.baseEntity().removeSpawnProtectionEffectRecursively();
        }
    }

    template <typename Hue>
    [[nodiscard]] cs2::Color getColor(std::optional<Hue> hue, color::Saturation saturation) const
    {
        if (hue.has_value())
            return color::HSBtoRGB(*hue, saturation, color::Brightness{1.0f});
        return model_glow_params::kFallbackColor;
    }

    [[nodiscard]] cs2::Color getColor(auto hue, color::Saturation saturation) const
    {
        return color::HSBtoRGB(hue, saturation, color::Brightness{1.0f});
    }

    [[nodiscard]] color::Saturation getSaturation(auto&& glow, auto&& entity) const
    {
        if constexpr (requires { { glow.saturation(entity) } -> std::same_as<color::Saturation>; })
            return glow.saturation(entity);
        else
            return color::Saturation{1.0f};
    }

    [[nodiscard]] cs2::Color getGlowHue(auto&& glow, auto&& entity, [[maybe_unused]] EntityTypeInfo entityTypeInfo) const
    {
        if constexpr (requires { { glow.getGlowHue(entity) }; }) {
            return getColor(glow.getGlowHue(entity), getSaturation(glow, entity));
        } else if constexpr (requires { { glow.getGlowHue(entityTypeInfo) }; }) {
            return getColor(glow.getGlowHue(entityTypeInfo), getSaturation(glow, entity));
        } else {
            return getColor(glow.getGlowHue(), getSaturation(glow, entity));
        }
    }

    void hookSceneObjectUpdater(auto&& glow, auto&& entity) const noexcept
    {
        entity.setSceneObjectUpdater(glow.replacementSceneObjectUpdater());
    }

    [[nodiscard]] bool hasSceneObjectUpdaterHooked(auto&& glow, auto&& entity) const noexcept
    {
        return entity.getSceneObjectUpdater() == glow.replacementSceneObjectUpdater();
    }

    void storeOriginalSceneObjectUpdater(auto&& glow, auto&& entity) const noexcept
    {
        auto& originalSceneObjectUpdater = glow.originalSceneObjectUpdater();
        if (originalSceneObjectUpdater == nullptr)
            originalSceneObjectUpdater = entity.getSceneObjectUpdater();
    }

    [[nodiscard]] auto& state() const noexcept
    {
        return hookContext.featuresStates().visualFeaturesStates.modelGlowState;
    }

    HookContext& hookContext;
};
