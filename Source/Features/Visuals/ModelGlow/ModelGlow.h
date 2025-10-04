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
            if (!modelGlowEnabled() && !state().modelGlowDisabling)
                return;
            if (!glow.enabled() && !glow.disablingFlag())
                return;

            if (modelGlowEnabled() && glow.enabled() && shouldApplyGlow(glow, entity))
                applyGlowInMainThread(glow, entityTypeInfo, entity);
            else
                removeGlowInMainThread(glow, entity);
        };
    }

    [[nodiscard]] auto updateInSceneObjectUpdater() const noexcept
    {
        return [this](auto&& glow, auto&& entity, EntityTypeInfo entityTypeInfo) {
            static_assert(isUsingSceneObjectUpdaterHook<decltype(glow)>());
            if (modelGlowEnabled() && glow.enabled() && shouldApplyGlow(glow, entity))
                entity.baseEntity().applySpawnProtectionEffectRecursively(getGlowHue(glow, entity, entityTypeInfo));
        };
    }

    [[nodiscard]] auto onUnload() const noexcept
    {
        return [this](auto&& glow, auto&& entity) {
            assert(state().modelGlowDisabling == false && "Should be already disabled");
            assert(glow.disablingFlag() == false && "Should be already disabled");

            if (modelGlowEnabled() && glow.enabled())
                removeGlowInMainThread(glow, entity);
        };
    }

    void postUpdateInMainThread() const
    {
        state().modelGlowDisabling = false;
        state().playerModelGlowDisabling = false;
        state().weaponModelGlowDisabling = false;
        state().droppedBombModelGlowDisabling = false;
        state().tickingBombModelGlowDisabling = false;
        state().defuseKitModelGlowDisabling = false;
        state().grenadeProjectileModelGlowDisabling = false;
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
        if constexpr (isUsingSceneObjectUpdaterHook<decltype(glow)>())
            hookSceneObjectUpdater(entity, glow);
        else
            entity.baseEntity().applySpawnProtectionEffectRecursively(getGlowHue(glow, entity, entityTypeInfo));
    }

    void removeGlowInMainThread(auto&& glow, auto&& entity) const
    {
        if constexpr (isUsingSceneObjectUpdaterHook<decltype(glow)>())
            restoreSceneObjectUpdater(entity, glow);
        else
            entity.baseEntity().removeSpawnProtectionEffectRecursively();
    }

    template <typename Hue>
    [[nodiscard]] cs2::Color getColor(Optional<Hue> hue, color::Saturation saturation) const
    {
        if (hue.hasValue())
            return color::HSBtoRGB(hue.value(), saturation, color::Brightness{1.0f});
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
        if constexpr (requires { { glow.hue(entity) }; }) {
            return getColor(glow.hue(entity), getSaturation(glow, entity));
        } else if constexpr (requires { { glow.hue(entityTypeInfo) }; }) {
            return getColor(glow.hue(entityTypeInfo), getSaturation(glow, entity));
        } else {
            return getColor(glow.hue(), getSaturation(glow, entity));
        }
    }

    void hookSceneObjectUpdater(auto&& entity, auto&& glow) const
    {
        if (entity.getSceneObjectUpdater() != glow.replacementSceneObjectUpdater()) {
            storeOriginalSceneObjectUpdater(glow, entity);
            entity.setSceneObjectUpdater(glow.replacementSceneObjectUpdater());
        }
    }

    void restoreSceneObjectUpdater(auto&& entity, auto&& glow) const
    {
        if (entity.getSceneObjectUpdater() == glow.replacementSceneObjectUpdater())
            entity.setSceneObjectUpdater(glow.originalSceneObjectUpdater());
    }

    void storeOriginalSceneObjectUpdater(auto&& glow, auto&& entity) const
    {
        auto& originalSceneObjectUpdater = glow.originalSceneObjectUpdater();
        assert(originalSceneObjectUpdater == nullptr || originalSceneObjectUpdater == entity.getSceneObjectUpdater());
        if (originalSceneObjectUpdater == nullptr)
            originalSceneObjectUpdater = entity.getSceneObjectUpdater();
    }

    [[nodiscard]] auto& state() const
    {
        return hookContext.featuresStates().visualFeaturesStates.modelGlowState;
    }

    [[nodiscard]] bool modelGlowEnabled() const
    {
        return hookContext.config().template getVariable<model_glow_vars::Enabled>();
    }

    HookContext& hookContext;
};
