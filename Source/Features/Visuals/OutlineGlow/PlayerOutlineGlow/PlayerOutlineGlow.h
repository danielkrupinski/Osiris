#pragma once

#include <utility>

#include <CS2/Classes/Color.h>
#include <CS2/Constants/ColorConstants.h>
#include <GameClient/Entities/EntityClassifier.h>
#include <GameClient/Entities/TeamNumber.h>
#include <Utils/ColorUtils.h>
#include "PlayerOutlineGlowColorType.h"

template <typename HookContext>
class PlayerOutlineGlow {
public:
    explicit PlayerOutlineGlow(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    [[nodiscard]] bool shouldApplyGlow(EntityTypeInfo /* entityTypeInfo */, auto&& playerPawn) const noexcept
    {
        return enabled() && shouldGlowPlayer(playerPawn);
    }

    [[nodiscard]] Optional<color::Hue> getGlowHue(EntityTypeInfo /* entityTypeInfo */, auto&& playerPawn) const noexcept
    {
        using namespace outline_glow_params;

        const auto colorMode = hookContext.config().template getVariable<PlayerOutlineGlowColorMode>();
        if (colorMode == PlayerOutlineGlowColorType::HealthBased) {
            if (const auto playerHealth = playerPawn.health(); playerHealth.hasValue())
                return healthColor(healthFraction(playerHealth.value()));
            return {};
        } else if (colorMode == PlayerOutlineGlowColorType::PlayerOrTeamColor) {
            if (const auto playerColorHue = getPlayerColorHue(playerPawn.playerController().playerColorIndex()))
                return playerColorHue->toHueFloat();
        }

        switch (playerPawn.teamNumber()) {
        case TeamNumber::TT: return kTeamTHue.toHueFloat();
        case TeamNumber::CT: return kTeamCTHue.toHueFloat();
        default: return {};
        };
    }

    [[nodiscard]] std::uint8_t getGlowColorAlpha(auto&& playerPawn) const noexcept
    {
        using namespace outline_glow_params;
        return playerPawn.hasImmunity().valueOr(false) ? kImmunePlayerGlowAlpha : kGlowAlpha;
    }

private:
    [[nodiscard]] bool enabled() const noexcept
    {
        return hookContext.config().template getVariable<PlayerOutlineGlowEnabled>();
    }

    [[nodiscard]] bool shouldGlowPlayer(auto&& playerPawn) const noexcept
    {
        return playerPawn.isAlive().value_or(true)
            && playerPawn.health().greaterThan(0).valueOr(true)
            && !playerPawn.isControlledByLocalPlayer()
            && playerPawn.isTTorCT()
            && (!hookContext.config().template getVariable<PlayerOutlineGlowOnlyEnemies>() || playerPawn.isEnemy().value_or(true));
    }

    [[nodiscard]] static color::Hue healthColor(float healthFraction) noexcept
    {
        return color::Hue{color::kRedHue + (color::kGreenHue - color::kRedHue) * healthFraction};
    }

    [[nodiscard]] static constexpr float healthFraction(int playerHealth) noexcept
    {
        constexpr auto kMinHealth = 1;
        constexpr auto kMaxHealth = 100;

        if (playerHealth <= kMinHealth)
            return 0.0f;
        if (playerHealth >= kMaxHealth)
            return 1.0f;
        return static_cast<float>(playerHealth - kMinHealth) / (kMaxHealth - kMinHealth); 
    }

    [[nodiscard]] std::optional<color::HueInteger> getPlayerColorHue(auto playerColorIndex) const noexcept
    {
        if (!playerColorIndex.hasValue())
            return {};

        switch (playerColorIndex.value()) {
        using namespace outline_glow_params;
        case 0: return kPlayerBlueHue;
        case 1: return kPlayerGreenHue;
        case 2: return kPlayerYellowHue;
        case 3: return kPlayerOrangeHue;
        case 4: return kPlayerPurpleHue;
        default: return {};
        }
    }

    HookContext& hookContext;
};
