#pragma once

#include <CS2/Panorama/CDropDown.h>
#include <MemoryPatterns/PatternTypes/PanoramaDropDownPatternTypes.h>

template <typename HookContext>
class DropdownMenu {
public:
    DropdownMenu(HookContext& hookContext, cs2::CDropDownMenu* dropdownMenu) noexcept
        : hookContext{hookContext}
        , dropdownMenu{dropdownMenu}
    {
    }

    using RawType = cs2::CDropDownMenu;

    [[nodiscard]] int getSelectedIndex() const
    {
        const auto selectedOption = getSelected();
        if (!selectedOption)
            return -1;

        int i = 0;
        for (auto&& child : uiPanel().children()) {
            if (child == selectedOption)
                return i;
            ++i;
        }
        return -1;
    }

private:
    [[nodiscard]] cs2::CUIPanel* getSelected() const
    {
        return hookContext.template make<ClientPanel>(dropdownMenu ? dropdownMenu->selectedChild : nullptr).uiPanel();
    }

    [[nodiscard]] decltype(auto) uiPanel() const
    {
        return hookContext.uiPanel(dropdownMenu ? dropdownMenu->uiPanel : nullptr);
    }

    HookContext& hookContext;
    cs2::CDropDownMenu* dropdownMenu;
};

template <typename HookContext>
class PanoramaDropDown {
public:
    PanoramaDropDown(HookContext& hookContext, cs2::CDropDown* dropDown) noexcept
        : hookContext{hookContext}
        , dropDown{dropDown}
    {
    }

    using RawType = cs2::CDropDown;

    [[nodiscard]] int getSelectedIndex() const
    {
        return dropdownMenu().getSelectedIndex();
    }

    void setSelectedIndex(int index) const noexcept
    {
        const auto setSelectedIndexFn = hookContext.patternSearchResults().template get<SetSelectedIndexFunctionPointer>();
        if (dropDown && setSelectedIndexFn)
            setSelectedIndexFn(dropDown, index);
    }

    void registerSelectionChangedHandler(cs2::CDropDown::DropDownSelectionChangedHandler* func)
    {
        auto&& uiEngine = hookContext.template make<PanoramaUiEngine>();
        uiEngine.registerEventHandler(
            uiEngine.makeSymbol(0, "DropDownSelectionChanged"),
            uiPanel(),
            cs2::CUtlAbstractDelegate{func}
        );
    }

private:
    [[nodiscard]] decltype(auto) dropdownMenu() const
    {
        if (dropDown)
            return hookContext.template make<PanoramaUiEngine>().getPanelFromHandle(dropDown->dropdownMenu).clientPanel().template as<DropdownMenu>();
        return hookContext.template make<DropdownMenu>(nullptr);
    }

    [[nodiscard]] decltype(auto) uiPanel() const
    {
        return hookContext.uiPanel(dropDown ? dropDown->uiPanel : nullptr);
    }

    HookContext& hookContext;
    cs2::CDropDown* dropDown;
};
