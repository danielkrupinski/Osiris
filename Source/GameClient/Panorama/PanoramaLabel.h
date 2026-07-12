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
#if IS_WIN64()
        (void)trustedSource;
        if (!panel)
            return;

        auto* const panelBytes = reinterpret_cast<std::byte*>(panel);
        auto* const panelVmt = *reinterpret_cast<void***>(panel);
        if (!*reinterpret_cast<void**>(panelBytes + 0x80)) {
            reinterpret_cast<void (*)(cs2::CLabel*)>(panelVmt[0x2A0 / sizeof(void*)])(panel);
        } else if (const auto getTextFormatter = hookContext.patternSearchResults().template get<GetLabelTextFormatterFunctionPointer>()) {
            auto* const textStorage = panelBytes + (textType == 3 ? 0x88 : 0x78);
            if (auto* const formatter = getTextFormatter(textStorage)) {
                auto* const formatterVmt = *reinterpret_cast<void***>(formatter);
                reinterpret_cast<void (*)(void*, const char*)>(formatterVmt[0x48 / sizeof(void*)])(formatter, value);
            }
        }

        panelBytes[0x28] = std::byte{1};
        if (panel->uiPanel) {
            auto* const uiPanelVmt = *reinterpret_cast<void***>(panel->uiPanel);
            reinterpret_cast<void (*)(cs2::CUIPanel*)>(uiPanelVmt[0x298 / sizeof(void*)])(panel->uiPanel);
        }
#else
        if (setTextInternalFunction())
            setTextInternalFunction()(panel, value, textType, trustedSource);
#endif
    }

private:
    [[nodiscard]] auto setTextInternalFunction() const noexcept
    {
        return hookContext.patternSearchResults().template get<SetLabelTextFunctionPointer>();
    }

    HookContext& hookContext;
    cs2::CLabel* panel;
};
