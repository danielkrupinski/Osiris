#pragma once

#include <CS2/Classes/VMatrix.h>
#include <CS2/Constants/AspectRatio.h>
#include <MemoryPatterns/PatternTypes/ClientPatternTypes.h>
#include <Utils/Math.h>

template <typename HookContext>
class ViewToProjectionMatrix {
public:
    explicit ViewToProjectionMatrix(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    [[nodiscard]] float getFovScale() const noexcept
    {
        if (matrix())
            return matrix()->m[1][1] / cs2::kDefaultAspectRatio;
        return 1.0f;
    }

    [[nodiscard]] float getAspectRatio() const noexcept
    {
        const auto viewMatrix = matrix();
        if (viewMatrix && viewMatrix->m[0][0] != 0.0f) {
            const float aspect = viewMatrix->m[1][1] / viewMatrix->m[0][0];
            if (Math::isFinite(aspect) && aspect > 0.0f)
                return aspect;
        }
        return cs2::kDefaultAspectRatio;
    }

    [[nodiscard]] std::optional<float> transformY(float yInViewSpace) const noexcept
    {
        if (matrix())
            return yInViewSpace * matrix()->m[1][1];
        return {};
    }

private:
    [[nodiscard]] decltype(auto) matrix() const
    {
        return hookContext.patternSearchResults().template get<ViewToProjectionMatrixPointer>();
    }

    HookContext& hookContext;
};
