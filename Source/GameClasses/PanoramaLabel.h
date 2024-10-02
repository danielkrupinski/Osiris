#pragma once

#include <CS2/Panorama/CLabel.h>
#include <GameDependencies/PanoramaLabelDeps.h>

template <typename HookContext>
struct PanoramaLabel {
    PanoramaLabel(HookContext& hookContext, cs2::CPanel2D* panel) noexcept
        : hookContext{hookContext}
        , panel{static_cast<cs2::CLabel*>(panel)}
    {
    }

    [[nodiscard]] decltype(auto) uiPanel() const noexcept
    {
        return hookContext.template make<PanoramaUiPanel>(panel->uiPanel);
    }

    void setText(const char* value) const noexcept
    {
        setTextInternal(value, 0, true);
    }

    void setTextInternal(const char* value, int textType, bool trustedSource) const noexcept
    {
        if (panel && deps().setTextInternal)
            deps().setTextInternal(panel, value, textType, trustedSource);
    }

private:
    [[nodiscard]] const auto& deps() const noexcept
    {
        return hookContext.gameDependencies().panoramaLabelDeps;
    }

    HookContext& hookContext;
    cs2::CLabel* panel;
};
