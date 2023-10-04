#pragma once

#include <cstdint>

#include <CS2/Classes/Color.h>
#include <CS2/Classes/Panorama.h>

struct PanelStyle {
    explicit PanelStyle(cs2::CPanelStyle* thisptr) noexcept
        : thisptr{ thisptr }
    {
    }

    explicit operator bool() const noexcept
    {
        return thisptr != nullptr;
    }

    void setSimpleForegroundColor(cs2::Color color) const noexcept
    {
        // FIXME: hardcoded virtual method index
        reinterpret_cast<void(*)(cs2::CPanelStyle* thisptr, const cs2::Color* color)>((*reinterpret_cast<void(***)()>(thisptr))[54])(thisptr, &color);
    }

private:
    cs2::CPanelStyle* thisptr;
};
