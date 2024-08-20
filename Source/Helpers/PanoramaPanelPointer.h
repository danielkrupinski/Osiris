#pragma once

#include <cassert>

#include <CS2/Panorama/CUIPanel.h>
#include <CS2/Panorama/PanelHandle.h>
#include <GameClasses/PanoramaUiEngine.h>
#include <GameClasses/PanoramaUiPanel.h>

struct PanoramaPanelPointer {
    explicit(false) PanoramaPanelPointer(cs2::CUIPanel* panel) noexcept
        : handle{ PanoramaUiEngine::getPanelHandle(panel) }
    {
        assert(!panel || handle.isValid());
    }

    PanoramaPanelPointer() = default;

    [[nodiscard]] cs2::PanelHandle getHandle() const noexcept
    {
        return handle;
    }

    [[nodiscard]] cs2::CUIPanel* get() const noexcept
    {
        return PanoramaUiEngine::getPanelPointer(handle);
    }

    cs2::PanelHandle handle;
};
