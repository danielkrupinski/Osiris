#pragma once

#include <cassert>

#include "ClipSpaceCoordinates.h"
#include <CS2/Classes/Vector.h>
#include <CS2/Classes/VMatrix.h>
#include <MemoryPatterns/ClientPatterns.h>

struct WorldToClipSpaceConverter {
    [[nodiscard]] explicit operator bool() const noexcept
    {
        return worldToProjectionMatrix != nullptr;
    }

    [[nodiscard]] ClipSpaceCoordinates toClipSpace(const cs2::Vector& worldPosition) const noexcept
    {
        assert(static_cast<bool>(*this));

        const auto& matrix = *worldToProjectionMatrix;
        return ClipSpaceCoordinates{
            .x = matrix.m[0][0] * worldPosition.x + matrix.m[0][1] * worldPosition.y + matrix.m[0][2] * worldPosition.z + matrix.m[0][3],
            .y = matrix.m[1][0] * worldPosition.x + matrix.m[1][1] * worldPosition.y + matrix.m[1][2] * worldPosition.z + matrix.m[1][3],
            .z = matrix.m[2][0] * worldPosition.x + matrix.m[2][1] * worldPosition.y + matrix.m[2][2] * worldPosition.z + matrix.m[2][3],
            .w = matrix.m[3][0] * worldPosition.x + matrix.m[3][1] * worldPosition.y + matrix.m[3][2] * worldPosition.z + matrix.m[3][3]
        };
    }

private:
    const cs2::VMatrix* worldToProjectionMatrix{ ClientPatterns::worldToProjectionMatrix() };
};
