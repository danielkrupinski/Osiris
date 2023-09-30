#pragma once

#include <MemoryPatterns/PanoramaLabelPatterns.h>

struct PanoramaLabelImpl {
    [[nodiscard]] static const PanoramaLabelImpl& instance() noexcept;

    cs2::CLabel::setTextInternal setTextInternal{ PanoramaLabelPatterns::setTextInternal() };
};
