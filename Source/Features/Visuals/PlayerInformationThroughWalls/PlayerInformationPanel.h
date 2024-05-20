#pragma once

#include <CS2/Classes/Panorama.h>
#include <GameClasses/PanoramaUiPanel.h>

struct PlayerInformationPanel {
    explicit PlayerInformationPanel(PanoramaUiPanel parentPanel) noexcept
    {
        retrieveChildren(parentPanel);
    }

    [[nodiscard]] bool isValid() const noexcept
    {
        return positionArrowPanel != nullptr;
    }

    cs2::CUIPanel* positionArrowPanel{};
    cs2::CUIPanel* healthPanel;
    cs2::CUIPanel* weaponIconPanel;
    cs2::CUIPanel* weaponAmmoPanel;
    cs2::CUIPanel* playerStateIconsPanel;

private:
    void retrieveChildren(PanoramaUiPanel parentPanel) noexcept
    {
        const auto children = parentPanel.children();
        if (!children || children->size != 5)
            return;

        positionArrowPanel = children->memory[0];
        healthPanel = children->memory[1];
        weaponIconPanel = children->memory[2];
        weaponAmmoPanel = children->memory[3];
        playerStateIconsPanel = children->memory[4];
    }
};
