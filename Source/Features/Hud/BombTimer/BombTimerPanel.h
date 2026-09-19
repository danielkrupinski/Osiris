#pragma once

#include <CS2/Constants/BombsiteIndex.h>
#include <CS2/Constants/IconURLs.h>
#include <GameClient/Panorama/ImagePanel.h>
#include <GameClient/Panorama/PanelHandle.h>
#include <GameClient/Panorama/PanoramaLabel.h>
#include <GameClient/Panorama/PanoramaUiEngine.h>
#include <Utils/StringBuilder.h>
#include "BombTimerPanelFactory.h"

template <typename HookContext>
class BombTimerPanel {
public:
    explicit BombTimerPanel(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    void showAndUpdate(auto&& tickingC4) const
    {
        auto&& container = bombTimerContainerPanel();
        container.show();

        setBombsiteIcon(container.children()[0].clientPanel().template as<ImagePanel>(), getBombsiteIconUrl(tickingC4));
        setTimeToExplosion(container.children()[1].clientPanel().template as<PanoramaLabel>(), tickingC4.getTimeToExplosion().valueOr(0.0f));
    }

    void hide() const
    {
        uiEngine().getPanelFromHandle(state().bombTimerContainerPanelHandle).hide();
    }

private:
    void setBombsiteIcon(auto&& bombsiteIconPanel, const char* bombsiteIconUrl) const
    {
        if (bombsiteIconUrl)
            bombsiteIconPanel.setImageSvg(bombsiteIconUrl);
    }

    void setTimeToExplosion(auto&& bombTimerTextPanel, float timeToExplosion) const
    {
        StringBuilderStorage<10> storage;
        StringBuilder builder = storage.builder();
        builder.put(static_cast<int>(timeToExplosion), '.', static_cast<int>(timeToExplosion * 10) % 10);
        bombTimerTextPanel.setText(builder.cstring());
    }

    [[nodiscard]] const char* getBombsiteIconUrl(auto&& tickingC4) const
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

    [[nodiscard]] auto& state() const
    {
        return hookContext.featuresStates().hudFeaturesStates.bombTimerState;
    }

    [[nodiscard]] decltype(auto) bombTimerContainerPanel() const
    {
        return hookContext.template make<PanelHandle>(state().bombTimerContainerPanelHandle).getOrInit(createContainerPanel());
    }

    [[nodiscard]] auto createContainerPanel() const noexcept
    {
        return [this] () -> decltype(auto) {
            auto&& factory = hookContext.template make<BombTimerPanelFactory>();
            auto&& container = factory.createContainerPanel(hookContext.hud().scoreAndTimeAndBomb());
            factory.createBombsiteIconPanel(container);
            factory.createTimerTextPanel(container);
            return utils::lvalue<decltype(container)>(container);
        };
    }

    [[nodiscard]] decltype(auto) uiEngine() const
    {
        return hookContext.template make<PanoramaUiEngine>();
    }

    HookContext& hookContext;
};
