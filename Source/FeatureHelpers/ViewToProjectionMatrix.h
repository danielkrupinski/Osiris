#pragma once

#include <CS2/Classes/VMatrix.h>
#include <CS2/Constants/AspectRatio.h>

struct ViewToProjectionMatrix {
    template <typename ClientPatterns>
    explicit ViewToProjectionMatrix(const ClientPatterns& clientPatterns) noexcept
        : viewToProjectionMatrix{clientPatterns.viewToProjectionMatrix()}
    {
    }

    [[nodiscard]] float getFovScale() const noexcept
    {
        if (viewToProjectionMatrix)
            return viewToProjectionMatrix->m[1][1] / cs2::kDefaultAspectRatio;
        return 1.0f;
    }

private:
    const cs2::VMatrix* viewToProjectionMatrix;
};
