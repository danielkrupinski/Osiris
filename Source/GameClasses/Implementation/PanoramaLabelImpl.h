#pragma once

#include <MemoryPatterns/PanoramaLabelPatterns.h>

struct PanoramaLabelImpl {
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
