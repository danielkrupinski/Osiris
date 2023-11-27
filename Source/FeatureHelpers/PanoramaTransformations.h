#pragma once

#include <array>
#include <cstddef>

#include <CS2/Classes/CUtlVector.h>
#include <CS2/Classes/Panorama.h>
#include <GameClasses/PanelStyle.h>

template <std::size_t N>
struct PanoramaTransformations {
    void applyTo(PanelStyle style) noexcept
    {
        cs2::CUtlVector<cs2::CTransform3D*> dummyVector;
        dummyVector.allocationCount = N;
        dummyVector.memory = transformations.data();
        dummyVector.growSize = 0;
        dummyVector.size = N;

        style.setTransform3D(dummyVector);
    }

    std::array<cs2::CTransform3D*, N> transformations;
};

template <typename... Args>
PanoramaTransformations(Args...) -> PanoramaTransformations<sizeof...(Args)>;
