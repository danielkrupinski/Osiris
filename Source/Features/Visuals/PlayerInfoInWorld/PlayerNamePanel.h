#pragma once

#include <algorithm>
#include <optional>
#include <utility>

#include <CS2/Classes/Color.h>
#include <CS2/Constants/ColorConstants.h>
#include <CS2/Panorama/CUIPanel.h>
#include <Features/Visuals/PlayerInfoInWorld/PlayerInfoInWorldConfigVariables.h>
#include <GameClient/Entities/TeamNumber.h>
#include <GameClient/Panorama/PanoramaLabel.h>
#include <GameClient/Panorama/PanoramaUiPanel.h>
#include <Utils/ColorUtils.h>

#include "PlayerInfoPanelCacheEntry.h"

template <typename HookContext>
class PlayerNamePanelContext {
public:
    PlayerNamePanelContext(HookContext& hookContext, cs2::CUIPanel* panel, PlayerInfoPanelCacheEntry& cache) noexcept
        : _hookContext{hookContext}
        , _panel{panel}
        , _cache{cache}
    {
    }

    [[nodiscard]] decltype(auto) config() const noexcept
    {
        return _hookContext.config();
    }

    [[nodiscard]] decltype(auto) panel() const noexcept
    {
        return _hookContext.template make<PanoramaUiPanel>(_panel);
    }

    [[nodiscard]] auto& cache() const noexcept
    {
        return _cache;
    }

private:
    HookContext& _hookContext;
    cs2::CUIPanel* _panel;
    PlayerInfoPanelCacheEntry& _cache;
};

template <typename HookContext, typename Context = PlayerNamePanelContext<HookContext>>
class PlayerNamePanel {
public:
    template <typename... Args>
    explicit PlayerNamePanel(Args&&... args) noexcept
        : context{std::forward<Args>(args)...}
    {
    }

    void update(auto&& playerPawn) const noexcept
    {
        if (!context.config().template getVariable<player_info_vars::PlayerNameEnabled>()) {
            context.panel().setVisible(false);
            return;
        }

        const auto playerName = playerPawn.playerController().getName();
        if (!playerName || !*playerName) {
            context.panel().setVisible(false);
            return;
        }

        context.panel().setVisible(true);

        if (const auto nameColor = getColor(playerPawn); context.cache().playerNameColor(nameColor))
            context.panel().setColor(nameColor);

        context.panel().clientPanel().template as<PanoramaLabel>().setText(playerName);
    }

private:
    [[nodiscard]] cs2::Color getColor(auto&& playerPawn) const noexcept
    {
        switch (context.config().template getVariable<player_info_vars::PlayerNameColorMode>()) {
        case PlayerNameColorType::TeamColor: return teamColor(playerPawn);
        case PlayerNameColorType::HealthBased: return healthColor(playerPawn).value_or(cs2::kColorWhite);
        default: return cs2::kColorWhite;
        }
    }

    [[nodiscard]] static cs2::Color teamColor(auto&& playerPawn) noexcept
    {
        switch (playerPawn.teamNumber()) {
            using enum TeamNumber;
        case TT: return cs2::kColorTeamTT;
        case CT: return cs2::kColorTeamCT;
        default: return cs2::kColorWhite;
        }
    }

    [[nodiscard]] static std::optional<cs2::Color> healthColor(auto&& playerPawn) noexcept
    {
        if (const auto healthValue = playerPawn.health(); healthValue.hasValue())
            return color::HSBtoRGB(color::Hue{color::kRedHue + (color::kGreenHue - color::kRedHue) * (std::clamp(healthValue.value(), 0, 100) / 100.0f)}, color::Saturation{0.7f}, color::Brightness{1.0f});
        return {};
    }

    Context context;
};
