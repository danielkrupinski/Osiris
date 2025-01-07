#pragma once

#include <CS2/Panorama/CDropDown.h>
#include <MemoryPatterns/PatternTypes/PanoramaDropDownPatternTypes.h>

template <typename HookContext>
class PanoramaDropDown {
public:
    PanoramaDropDown(HookContext& hookContext, cs2::CDropDown* dropDown) noexcept
        : hookContext{hookContext}
        , dropDown{dropDown}
    {
    }

    using RawType = cs2::CDropDown;

    void setSelectedIndex(int index) const noexcept
    {
        const auto setSelectedIndexFn = hookContext.clientPatternSearchResults().template get<SetSelectedIndexFunctionPointer>();
        if (dropDown && setSelectedIndexFn)
            setSelectedIndexFn(dropDown, index);
    }

private:
    HookContext& hookContext;
    cs2::CDropDown* dropDown;
};
