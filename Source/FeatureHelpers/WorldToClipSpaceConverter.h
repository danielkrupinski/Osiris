#pragma once

#include <cassert>

#include "ClipSpaceCoordinates.h"
#include <CS2/Classes/Vector.h>
#include <CS2/Classes/VMatrix.h>

template <typename HookContext>
struct WorldToClipSpaceConverter {
    explicit WorldToClipSpaceConverter(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    [[nodiscard]] ClipSpaceCoordinates toClipSpace(const cs2::Vector& worldPosition) const noexcept
    {
        if (worldToProjectionMatrix()) {
            const auto& matrix = *worldToProjectionMatrix();
            return ClipSpaceCoordinates{
                .x = matrix.m[0][0] * worldPosition.x + matrix.m[0][1] * worldPosition.y + matrix.m[0][2] * worldPosition.z + matrix.m[0][3],
                .y = matrix.m[1][0] * worldPosition.x + matrix.m[1][1] * worldPosition.y + matrix.m[1][2] * worldPosition.z + matrix.m[1][3],
                .z = matrix.m[2][0] * worldPosition.x + matrix.m[2][1] * worldPosition.y + matrix.m[2][2] * worldPosition.z + matrix.m[2][3],
                .w = matrix.m[3][0] * worldPosition.x + matrix.m[3][1] * worldPosition.y + matrix.m[3][2] * worldPosition.z + matrix.m[3][3]
            };
        }
        return ClipSpaceCoordinates{.x = 0.0f, .y = 0.0f, .z = 0.0f, .w = 0.0f};
    }

private:
    [[nodiscard]] auto worldToProjectionMatrix() const noexcept
    {
        return hookContext.clientPatternSearchResults().template get<WorldToProjectionMatrixPointer>();
    }

    HookContext& hookContext;
};
