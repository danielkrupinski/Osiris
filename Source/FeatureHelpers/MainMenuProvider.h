#pragma once

#include <CS2/Classes/Panorama.h>
#include <GameClasses/PanoramaUiPanel.h>

struct MainMenuProvider {
    template <typename ClientPatterns>
    explicit MainMenuProvider(const ClientPatterns& clientPatterns) noexcept
        : mainMenu{clientPatterns.mainMenuPanel()}
    {
    }

    [[nodiscard]] PanoramaUiPanel getMainMenuPanel() const noexcept
    {
        if (mainMenu && *mainMenu)
            return PanoramaUiPanel{(*mainMenu)->uiPanel};
        return PanoramaUiPanel{nullptr};
    }

private:
    cs2::CPanel2D** mainMenu;
};
