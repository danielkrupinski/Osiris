#pragma once

#include <Features/Visuals/OutlineGlow/OutlineGlowConfigVariables.h>
#include <Features/Visuals/OutlineGlow/OutlineGlowParams.h>
#include <GameClient/Entities/EntityClassifier.h>
#include <Utils/ColorUtils.h>

template <typename HookContext>
class TickingBombOutlineGlow {
public:
    TickingBombOutlineGlow(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    [[nodiscard]] bool shouldApplyGlow(EntityTypeInfo /* entityTypeInfo */, auto&& plantedBomb) const noexcept
    {
        return enabled() && shouldGlowPlantedBomb(plantedBomb);
    }

    [[nodiscard]] Optional<color::Hue> getGlowHue(EntityTypeInfo /* entityTypeInfo */, auto&& /* plantedBomb */) const noexcept
    {
        return static_cast<color::HueInteger>(hookContext.config().template getVariable<OutlineGlowTickingBombHue>()).toHueFloat();
    }

private:
    [[nodiscard]] bool enabled() const noexcept
    {
        return hookContext.config().template getVariable<TickingBombOutlineGlowEnabled>();
    }

    [[nodiscard]] bool shouldGlowPlantedBomb(auto&& plantedBomb) const noexcept
    {
        return plantedBomb.isTicking().valueOr(true);
    }

    HookContext& hookContext;
};
