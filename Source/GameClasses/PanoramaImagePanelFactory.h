#pragma once

#include <CS2/Classes/Panorama.h>
#include <GameDependencies/PanoramaImagePanelDeps.h>

#include "MemAlloc.h"

struct PanoramaImagePanelFactory {
    [[nodiscard]] static cs2::CImagePanel* create(const char* id, cs2::CUIPanel* parent) noexcept
    {
        if (!PanoramaImagePanelDeps::instance().constructor || !PanoramaImagePanelDeps::instance().size)
            return nullptr;

        const auto memory{static_cast<cs2::CImagePanel*>(MemAlloc::allocate(*PanoramaImagePanelDeps::instance().size))};
        if (memory)
            PanoramaImagePanelDeps::instance().constructor(memory, parent->clientPanel, id);
        return memory;
    }
};
