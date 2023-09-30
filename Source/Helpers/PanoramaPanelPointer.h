#pragma once

#include <cassert>

#include <CS2/Classes/Panorama.h>
#include <GameClasses/PanoramaUiEngine.h>
#include <GameClasses/PanoramaUiPanel.h>

struct PanoramaPanelPointer {
    explicit(false) PanoramaPanelPointer(cs2::CUIPanel* panel) noexcept
        : handle{ PanoramaUiEngine::getPanelHandle(panel) }
    {
        assert(!panel || handle.isValid());
    }

    explicit(false) PanoramaPanelPointer(PanoramaUiPanel panel) noexcept
        : PanoramaPanelPointer{ static_cast<cs2::CUIPanel*>(panel) }
    {
    }

    PanoramaPanelPointer() = default;

    [[nodiscard]] cs2::PanelHandle getHandle() const noexcept
    {
        return handle;
    }

    [[nodiscard]] PanoramaUiPanel get() const noexcept
    {
        return PanoramaUiPanel{ PanoramaUiEngine::getPanelPointer(handle) };
    }

private:
    cs2::PanelHandle handle;
};
