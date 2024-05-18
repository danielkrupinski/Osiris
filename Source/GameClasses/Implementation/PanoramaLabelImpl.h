#pragma once

#include <cstdint>

#include <CS2/Classes/Panorama.h>

struct PanoramaLabelImpl {
    template <typename PanoramaLabelPatterns>
    explicit PanoramaLabelImpl(const PanoramaLabelPatterns& panoramaLabelPatterns) noexcept
        : setTextInternal{panoramaLabelPatterns.setTextInternal()}
        , constructor{panoramaLabelPatterns.constructor()}
        , size{panoramaLabelPatterns.size()}
    {
    }

    [[nodiscard]] static const PanoramaLabelImpl& instance() noexcept;

    cs2::CLabel::setTextInternal setTextInternal;
    cs2::CLabel::Constructor* constructor;
    std::uint32_t* size;
};
