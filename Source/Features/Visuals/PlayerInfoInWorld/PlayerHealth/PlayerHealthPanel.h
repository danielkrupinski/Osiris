#pragma once

#include <utility>

#include <CS2/Classes/Color.h>
#include <CS2/Constants/ColorConstants.h>
#include <CS2/Panorama/CUIPanel.h>
#include <Features/Visuals/PlayerInfoInWorld/PlayerInfoInWorldState.h>
#include <Features/Visuals/PlayerInfoInWorld/PlayerInfoPanelCacheEntry.h>
#include <GameClient/Panorama/PanoramaLabel.h>
#include <GameClient/Panorama/PanoramaUiPanel.h>
#include <Utils/StringBuilder.h>

template <typename HookContext>
class PlayerHealthPanel {
public:
    PlayerHealthPanel(HookContext& hookContext, cs2::CUIPanel* uiPanel, PlayerInfoPanelCacheEntry& cache) noexcept
        : hookContext{hookContext}
        , uiPanel{uiPanel}
        , cache{cache}
    {
    }

    void update(auto&& playerPawn) const noexcept
    {
        if (!GET_CONFIG_VAR(player_info_vars::PlayerHealthEnabled)) {
            panel().setVisible(false);
            return;
        }

        panel().setVisible(true);

        auto&& healthValuePanel = panel().children()[1];

        if (const auto healthTextColor = getColor(playerPawn); cache.playerHealthTextColor(healthTextColor))
            healthValuePanel.setColor(healthTextColor);

        if (const auto playerHealth = playerPawn.health().valueOr(0); cache.playerHealth(playerHealth))
            healthValuePanel.clientPanel().template as<PanoramaLabel>().setText(StringBuilderStorage<10>{}.builder().put(playerHealth).cstring());
    }

private:
    [[nodiscard]] cs2::Color getColor(auto&& playerPawn) const noexcept
    {
        if (GET_CONFIG_VAR(player_info_vars::PlayerHealthColorMode) == PlayerHealthTextColor::HealthBased)
            return healthColor(playerPawn).value_or(cs2::kColorWhite);
        return cs2::kColorWhite;
    }

    [[nodiscard]] std::optional<cs2::Color> healthColor(auto&& playerPawn, float saturation = 0.7f) const noexcept
    {
        if (const auto healthValue = playerPawn.health(); healthValue.hasValue())
            return getColorOfHealthFraction(saturation, std::clamp(healthValue.value(), 0, 100) / 100.0f);
        return {};
    }

    [[nodiscard]] static cs2::Color getColorOfHealthFraction(float saturation, float healthFraction) noexcept
    {
        return color::HSBtoRGB(color::Hue{color::kRedHue + (color::kGreenHue - color::kRedHue) * healthFraction}, color::Saturation{saturation}, color::Brightness{1.0f});
    }

    [[nodiscard]] decltype(auto) panel() const noexcept
    {
        return hookContext.template make<PanoramaUiPanel>(uiPanel);
    }

    HookContext& hookContext;
    cs2::CUIPanel* uiPanel;
    PlayerInfoPanelCacheEntry& cache;
};
