#pragma once

#include <CS2/Classes/Panorama.h>
#include <GameClasses/MemAlloc.h>

struct PanoramaLabelFactory {
    [[nodiscard]] cs2::CLabel* createLabelPanel(cs2::CUIPanel* parent, const char* id = "") const noexcept
    {
        const auto memory{static_cast<cs2::CLabel*>(MemAlloc::allocate(*size))};
        if (memory)
            constructor(memory, parent->clientPanel, id);
        return memory;
    }

    cs2::CLabel::Constructor* constructor;
    std::uint32_t* size;
};
