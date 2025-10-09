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

    void setFov(float fov) const noexcept
    {
        hookContext.patternSearchResults().template get<OffsetTo3dPanelFovWeight>().of(ui3dPanel) = 1.0f;
        hookContext.patternSearchResults().template get<OffsetTo3dPanelFov>().of(ui3dPanel) = fov;
    }

    template <template <typename...> typename T>
    [[nodiscard]] decltype(auto) as() const noexcept
    {
        return hookContext.template make<T>(static_cast<T<HookContext>::RawType*>(ui3dPanel));
    }

private:
    [[nodiscard]] decltype(auto) portraitWorldPointer() const noexcept
    {
        return hookContext.patternSearchResults().template get<OffsetToPortraitWorld>().of(ui3dPanel).valueOr(nullptr);
    }

    HookContext& hookContext;
    cs2::CUI_3dPanel* ui3dPanel;
};
