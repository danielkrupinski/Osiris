#pragma once

#include <CS2/Panorama/CLabel.h>
#include <CS2/Panorama/CPanel2D.h>
#include <MemoryPatterns/PatternTypes/PanoramaLabelPatternTypes.h>

#include "PanoramaUiPanel.h"

template <typename HookContext>
struct PanoramaLabel {
    using RawType = cs2::CLabel;

    PanoramaLabel(HookContext& hookContext, cs2::CPanel2D* panel) noexcept
        : hookContext{hookContext}
        , panel{static_cast<cs2::CLabel*>(panel)}
    {
    }

    [[nodiscard]] decltype(auto) uiPanel() const noexcept
    {
        return hookContext.template make<PanoramaUiPanel>(panel ? panel->uiPanel : nullptr);
    }

    void setText(const char* value) const noexcept
    {
        setTextInternal(value, 0, true);
    }

    void setTextInternal(const char* value, int textType, bool trustedSource) const noexcept
    {
        if (panel && setTextInternalFunction())
            setTextInternalFunction()(panel, value, textType, trustedSource);
    }

private:
    [[nodiscard]] auto setTextInternalFunction() const noexcept
    {
        return hookContext.clientPatternSearchResults().template get<SetLabelTextFunctionPointer>();
    }

    HookContext& hookContext;
    cs2::CLabel* panel;
};
