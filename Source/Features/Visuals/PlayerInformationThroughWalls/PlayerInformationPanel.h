#pragma once

#include <CS2/Panorama/CUIPanel.h>
#include <GameClasses/PanoramaUiPanel.h>

struct PlayerInformationPanel {
    explicit PlayerInformationPanel(auto parentPanel) noexcept
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
    void retrieveChildren(auto parentPanel) noexcept
    {
        const auto children = parentPanel.children();
        if (!children.vector || children.vector->size != 5)
            return;

        positionArrowPanel = children.vector->memory[0];
        healthPanel = children.vector->memory[1];
        weaponIconPanel = children.vector->memory[2];
        weaponAmmoPanel = children.vector->memory[3];
        playerStateIconsPanel = children.vector->memory[4];
    }
};
