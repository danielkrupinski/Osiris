#pragma once

#include <CS2/Panorama/CUI_3dPanel.h>
#include <GameClient/PortraitWorld.h>
#include <MemoryPatterns/PatternTypes/Ui3dPanelPatternTypes.h>

template <typename HookContext>
class Ui3dPanel {
public:
    Ui3dPanel(HookContext& hookContext, cs2::CUI_3dPanel* ui3dPanel) noexcept
        : hookContext{hookContext}
        , ui3dPanel{ui3dPanel}
    {
    }

    using RawType = cs2::CUI_3dPanel;

    [[nodiscard]] decltype(auto) portraitWorld() const noexcept
    {
        return hookContext.template make<PortraitWorld>(portraitWorldPointer());
    }

private:
    [[nodiscard]] decltype(auto) portraitWorldPointer() const noexcept
    {
        return hookContext.clientPatternSearchResults().template get<OffsetToPortraitWorld>().of(ui3dPanel).valueOr(nullptr);
    }

    HookContext& hookContext;
    cs2::CUI_3dPanel* ui3dPanel;
};
