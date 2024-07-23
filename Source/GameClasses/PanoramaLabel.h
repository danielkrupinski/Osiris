#pragma once

#include <CS2/Classes/Panorama.h>
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
        if (panel && PanoramaLabelDeps::instance().setTextInternal)
            PanoramaLabelDeps::instance().setTextInternal(panel, value, textType, trustedSource);
    }

private:
    HookContext& hookContext;
    cs2::CLabel* panel;
};
