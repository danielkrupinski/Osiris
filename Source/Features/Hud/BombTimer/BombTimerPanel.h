#pragma once

#include <CS2/Constants/BombsiteIndex.h>
#include <CS2/Constants/IconURLs.h>

template <typename Context>
struct BombTimerPanel {
    explicit BombTimerPanel(Context context) noexcept
        : context{context}
    {
    }

    void showAndUpdate() const noexcept
    {
        context.bombTimerContainerPanel().show();

        decltype(auto) tickingC4{context.tickingC4()};
        context.bombSiteIconPanel().setIcon(getBombSiteIconUrl(tickingC4));
        context.bombTimerTextPanel().setTimeToExplosion(tickingC4.getTimeToExplosion().valueOr(0.0f));
    }

    void hide() const noexcept
    {
        context.bombTimerContainerPanel().hide();
    }

private:
    [[nodiscard]] const char* getBombSiteIconUrl(auto&& tickingC4) const noexcept
    {
        auto&& bombsiteIndex = tickingC4.bombsiteIndex();
        if (!bombsiteIndex.hasValue())
            return nullptr;

        switch (bombsiteIndex.value()) {
        case cs2::BombsiteIndex::BombsiteA: return cs2::kBombSiteAIconUrl;
        case cs2::BombsiteIndex::BombsiteB: return cs2::kBombSiteBIconUrl;
        default: return nullptr;
        }
    }

    Context context;
};
