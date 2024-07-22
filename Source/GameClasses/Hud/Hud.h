#pragma once

#include <CS2/Constants/PanelIDs.h>

#include "DeathNotices.h"

template <typename Context>
struct Hud {
    explicit Hud(Context context) noexcept
        : context{context}
    {
    }

    [[nodiscard]] decltype(auto) deathNotices() noexcept
    {
        return context.deathNoticesPanelHandle().getOrInit(findVisibleDeathNoticesPanel()).template as<DeathNotices>();
    }

    [[nodiscard]] decltype(auto) getHudReticle() noexcept
    {
        return context.panel().findChildInLayoutFile(cs2::HudReticle);
    }

    [[nodiscard]] decltype(auto) scoreAndTimeAndBomb() noexcept
    {
        return context.scoreAndTimeAndBombPanelHandle().getOrInit(findScoreAndTimeAndBombPanel());
    }

    [[nodiscard]] decltype(auto) bombStatus() noexcept
    {
        return context.bombStatusPanelHandle().getOrInit(findBombStatusPanel());
    }

    [[nodiscard]] decltype(auto) hudTeamCounter() noexcept
    {
        return context.panel().findChildInLayoutFile(cs2::HudTeamCounter);
    }

private:
    [[nodiscard]] decltype(auto) hudDeathNotice() noexcept
    {
        return context.panel().findChildInLayoutFile(cs2::HudDeathNotice);
    }

    [[nodiscard]] auto findVisibleDeathNoticesPanel() noexcept
    {
        return [this] { 
            return hudDeathNotice().findChildInLayoutFile(cs2::VisibleNotices);
        };
    }

    [[nodiscard]] auto findBombStatusPanel() noexcept
    {
        return [this] {
            return scoreAndTimeAndBomb().findChildInLayoutFile(cs2::BombStatus);
        };
    }

    [[nodiscard]] auto findScoreAndTimeAndBombPanel() noexcept
    {
        return [this] {
            return hudTeamCounter().findChildInLayoutFile(cs2::ScoreAndTimeAndBomb);
        };
    }

    Context context;
};
