#pragma once

#include <cstdint>

#include <CS2/Panorama/CLabel.h>

struct PanoramaLabelDeps {
    template <typename PanoramaLabelPatterns>
    explicit PanoramaLabelDeps(const PanoramaLabelPatterns& panoramaLabelPatterns) noexcept
        : setTextInternal{panoramaLabelPatterns.setTextInternal()}
        , constructor{panoramaLabelPatterns.constructor()}
        , size{panoramaLabelPatterns.size()}
    {
    }

    [[nodiscard]] static const PanoramaLabelDeps& instance() noexcept;

    cs2::CLabel::setTextInternal setTextInternal;
    cs2::CLabel::Constructor* constructor;
    std::uint32_t* size;
};
