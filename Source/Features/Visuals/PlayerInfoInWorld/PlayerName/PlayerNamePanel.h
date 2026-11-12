#pragma once

#include <algorithm>
#include <optional>
#include <utility>

#include <CS2/Classes/Color.h>
#include <CS2/Constants/ColorConstants.h>
#include <CS2/Panorama/CUIPanel.h>
#include <Features/Visuals/PlayerInfoInWorld/PlayerInfoInWorldConfigVariables.h>
#include <Features/Visuals/PlayerInfoInWorld/PlayerInfoPanelCacheEntry.h>
#include <GameClient/Entities/TeamNumber.h>
#include <GameClient/Panorama/PanoramaLabel.h>
#include <GameClient/Panorama/PanoramaUiPanel.h>
#include <Utils/ColorUtils.h>

template <typename HookContext>
class PlayerNamePanel {
public:
    PlayerNamePanel(HookContext& hookContext, cs2::CUIPanel* uiPanel, PlayerInfoPanelCacheEntry& cache) noexcept
        : hookContext{hookContext}
        , uiPanel{uiPanel}
        , cache{cache}
    {
    }

    void update(auto&& playerPawn) const noexcept
    {
        if (!GET_CONFIG_VAR(player_info_vars::PlayerNameEnabled)) {
            panel().setVisible(false);
            return;
        }

        const auto playerName = playerPawn.playerController().getName();
        if (!playerName || !*playerName) {
            panel().setVisible(false);
            return;
        }

        panel().setVisible(true);

        if (const auto nameColor = getColor(playerPawn); cache.playerNameColor(nameColor))
            panel().setColor(nameColor);

        panel().clientPanel().template as<PanoramaLabel>().setText(playerName);
    }

private:
    [[nodiscard]] cs2::Color getColor(auto&& playerPawn) const noexcept
    {
        switch (GET_CONFIG_VAR(player_info_vars::PlayerNameColorMode)) {
            using enum PlayerNameColorType;
        case TeamColor:
            return teamColor(playerPawn);
        case HealthBased:
            return healthColor(playerPawn).value_or(cs2::kColorWhite);
        default:
            return cs2::kColorWhite;
        }
    }

    [[nodiscard]] static cs2::Color teamColor(auto&& playerPawn) noexcept
    {
        switch (playerPawn.teamNumber()) {
            using enum TeamNumber;
        case TT:
            return cs2::kColorTeamTT;
        case CT:
            return cs2::kColorTeamCT;
        default:
            return cs2::kColorWhite;
        }
    }

    [[nodiscard]] static std::optional<cs2::Color> healthColor(auto&& playerPawn) noexcept
    {
        if (const auto healthValue = playerPawn.health(); healthValue.hasValue())
            return color::HSBtoRGB(color::Hue{color::kRedHue + (color::kGreenHue - color::kRedHue) * (std::clamp(healthValue.value(), 0, 100) / 100.0f)}, color::Saturation{0.7f}, color::Brightness{1.0f});
        return std::nullopt;
    }

    [[nodiscard]] decltype(auto) panel() const noexcept
    {
        return hookContext.template make<PanoramaUiPanel>(uiPanel);
    }

    HookContext& hookContext;
    cs2::CUIPanel* uiPanel;
    PlayerInfoPanelCacheEntry& cache;
};
