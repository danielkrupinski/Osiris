#pragma once

#include <CS2/Panorama/CUI_Item3dPanel.h>
#include <MemoryPatterns/PatternTypes/UiItem3dPanelPatternTypes.h>

template <typename HookContext>
class UiItem3dPanel {
public:
    UiItem3dPanel(HookContext& hookContext, cs2::CUI_Item3dPanel* uiItem3dPanel) noexcept
        : hookContext{hookContext}
        , uiItem3dPanel{uiItem3dPanel}
    {
    }

    using RawType = cs2::CUI_Item3dPanel;

    void createItem(cs2::ItemId itemId) const noexcept
    {
        setItemId(itemId);
    }

    void startWeaponLookAt() const noexcept
    {
        hookContext.patternSearchResults().template get<OffsetToStartWeaponLookAt>().of(uiItem3dPanel) = true;
    }

private:
    void setItemId(cs2::ItemId itemId) const noexcept
    {
        if (const auto setItemItemIdFunction = hookContext.patternSearchResults().template get<SetItemItemIdFunction>(); setItemItemIdFunction && uiItem3dPanel)
            setItemItemIdFunction(uiItem3dPanel, itemId, "");
    }

    HookContext& hookContext;
    cs2::CUI_Item3dPanel* uiItem3dPanel;
};
