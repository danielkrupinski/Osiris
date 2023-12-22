#pragma once

#include <MemoryPatterns/PanoramaLabelPatterns.h>

struct PanoramaLabelImpl {
    explicit PanoramaLabelImpl(const PanoramaLabelPatterns& panoramaLabelPatterns) noexcept
        : setTextInternal{panoramaLabelPatterns.setTextInternal()}
    {
    }

    [[nodiscard]] static const PanoramaLabelImpl& instance() noexcept;

    cs2::CLabel::setTextInternal setTextInternal;
};
