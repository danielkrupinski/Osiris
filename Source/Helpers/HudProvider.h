#pragma once

#include <CS2/Classes/Panorama.h>
#include <CS2/Constants/PanelIDs.h>

#include <GameClasses/PanoramaUiPanel.h>

struct HudCache {
    cs2::CPanel2D* hudPanel{nullptr};
};

struct Hud {
    HudCache& cache;

    [[nodiscard]] cs2::CUIPanel* getHudReticle() const noexcept
    {
        return findChildInLayoutFile(cs2::HudReticle);
    }

    [[nodiscard]] PanoramaUiPanel hudTeamCounter() const noexcept
    {
        return findChildInLayoutFile(cs2::HudTeamCounter);
    }

    [[nodiscard]] PanoramaUiPanel hudDeathNotice() const noexcept
    {
        return findChildInLayoutFile(cs2::HudDeathNotice);
    }

private:
    [[nodiscard]] PanoramaUiPanel findChildInLayoutFile(const char* childId) const noexcept
    {
        return PanoramaUiPanel{cache.hudPanel->uiPanel}.findChildInLayoutFile(childId);
    }
};

struct HudOptional {
    HudOptional(cs2::CPanel2D** pointerToHud, HudCache& hudCache) noexcept
        : pointerToHud{pointerToHud}
        , hud{hudCache}
    {
    }

    cs2::CPanel2D** pointerToHud;
    Hud hud;

    [[nodiscard]] operator bool() const noexcept
    {
        return hud.cache.hudPanel || readHudPanel();
    }

    [[nodiscard]] const Hud* operator->() const noexcept
    {
        assert(static_cast<bool>(*this));
        return &hud;
    }

private:
    [[nodiscard]] bool readHudPanel() const noexcept
    {
        return pointerToHud && ((hud.cache.hudPanel = *pointerToHud) != nullptr);
    }
};
