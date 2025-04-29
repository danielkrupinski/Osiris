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

    [[nodiscard]] cs2::Color getGlowColor(EntityTypeInfo /* entityTypeInfo */, auto&& playerPawn) const noexcept
    {
        return getColor(playerPawn).setAlpha(getColorAlpha(playerPawn));
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

    [[nodiscard]] std::uint8_t getColorAlpha(auto&& playerPawn) const noexcept
    {
        using namespace outline_glow_params;
        return playerPawn.hasImmunity().valueOr(false) ? kImmunePlayerGlowAlpha : kGlowAlpha;
    }

    [[nodiscard]] static cs2::Color healthColor(float healthFraction) noexcept
    {
        return color::HSBtoRGB(color::Hue{color::kRedHue + (color::kGreenHue - color::kRedHue) * healthFraction}, outline_glow_params::kPlayerGlowSaturation, color::Brightness{1.0f});
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
    
    [[nodiscard]] cs2::Color getColor(auto&& playerPawn) const noexcept
    {
        using namespace outline_glow_params;

        const auto colorMode = hookContext.config().template getVariable<PlayerOutlineGlowColorMode>();
        if (colorMode == PlayerOutlineGlowColorType::HealthBased) {
            if (const auto playerHealth = playerPawn.health(); playerHealth.hasValue())
                return healthColor(healthFraction(playerHealth.value()));
            return kPlayerGlowFallbackColor;
        } else if (colorMode == PlayerOutlineGlowColorType::PlayerOrTeamColor) {
            if (const auto playerColorIndex = playerPawn.playerController().playerColorIndex(); playerColorIndex.hasValue() && playerColorIndex.value() >= 0 && std::cmp_less(playerColorIndex.value(), kPlayerColors.size()))
                return kPlayerColors[playerColorIndex.value()];
        }

        switch (playerPawn.teamNumber()) {
        case TeamNumber::TT: return kPlayerGlowColorTeamT;
        case TeamNumber::CT: return kPlayerGlowColorTeamCT;
        default: return kPlayerGlowFallbackColor;
        }
    }

    HookContext& hookContext;
};
