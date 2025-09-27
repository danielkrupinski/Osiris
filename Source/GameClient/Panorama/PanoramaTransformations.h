#pragma once

#include <array>
#include <cstddef>

#include <CS2/Classes/CUtlVector.h>
#include <CS2/Panorama/Transform3D.h>

template <std::size_t N>
struct PanoramaTransformations {
    void applyTo(auto&& panel) noexcept
    {
        panel.setTransform3D(transformations);
    }

    std::array<cs2::CTransform3D*, N> transformations;
};

template <typename... Args>
PanoramaTransformations(Args...) -> PanoramaTransformations<sizeof...(Args)>;
