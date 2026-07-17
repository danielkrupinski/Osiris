#pragma once

#include <GameClient/WorldToScreen/ClipSpaceCoordinates.h>
#include <Utils/Math.h>

struct TrajectoryLineSegment {
    float midpointX;
    float midpointY;
    float width;
    float angleDegrees;

    [[nodiscard]] static bool fromClipSpace(ClipSpaceCoordinates first, ClipSpaceCoordinates second, float aspectRatio, TrajectoryLineSegment& result) noexcept
    {
        constexpr float kNearW = 0.001f;
        if (!Math::isFinite(first.x) || !Math::isFinite(first.y) || !Math::isFinite(first.z) || !Math::isFinite(first.w)
            || !Math::isFinite(second.x) || !Math::isFinite(second.y) || !Math::isFinite(second.z) || !Math::isFinite(second.w)
            || !Math::isFinite(aspectRatio) || aspectRatio <= kNearW)
            return false;
        if (first.w < kNearW && second.w < kNearW)
            return false;

        if (first.w < kNearW || second.w < kNearW) {
            const float t = (kNearW - first.w) / (second.w - first.w);
            if (!Math::isFinite(t))
                return false;
            ClipSpaceCoordinates clipped{
                .x = first.x + (second.x - first.x) * t,
                .y = first.y + (second.y - first.y) * t,
                .z = first.z + (second.z - first.z) * t,
                .w = kNearW
            };
            if (!Math::isFinite(clipped.x) || !Math::isFinite(clipped.y) || !Math::isFinite(clipped.z))
                return false;
            if (first.w < kNearW)
                first = clipped;
            else
                second = clipped;
        }

        float x0 = first.x / first.w * 0.5f + 0.5f;
        float y0 = 0.5f - first.y / first.w * 0.5f;
        float x1 = second.x / second.w * 0.5f + 0.5f;
        float y1 = 0.5f - second.y / second.w * 0.5f;
        if (!Math::isFinite(x0) || !Math::isFinite(y0) || !Math::isFinite(x1) || !Math::isFinite(y1))
            return false;
        float dx = x1 - x0;
        float dy = y1 - y0;
        if (!Math::isFinite(dx) || !Math::isFinite(dy))
            return false;
        float enter = 0.0f;
        float leave = 1.0f;
        if (!clip(-dx, x0, enter, leave) || !clip(dx, 1.0f - x0, enter, leave)
            || !clip(-dy, y0, enter, leave) || !clip(dy, 1.0f - y0, enter, leave))
            return false;

        const float clippedX0 = x0 + dx * enter;
        const float clippedY0 = y0 + dy * enter;
        const float clippedX1 = x0 + dx * leave;
        const float clippedY1 = y0 + dy * leave;
        if (!Math::isFinite(clippedX0) || !Math::isFinite(clippedY0) || !Math::isFinite(clippedX1) || !Math::isFinite(clippedY1))
            return false;
        dx = clippedX1 - clippedX0;
        dy = clippedY1 - clippedY0;
        if (!Math::isFinite(dx) || !Math::isFinite(dy))
            return false;
        const float physicalDy = dy / aspectRatio;
        const float squaredLength = dx * dx + physicalDy * physicalDy;
        if (!Math::isFinite(physicalDy) || !Math::isFinite(squaredLength))
            return false;
        const float length = Math::sqrt(squaredLength);
        if (!Math::isFinite(length) || length <= 0.00001f)
            return false;

        result = {.midpointX = (clippedX0 + clippedX1) * 50.0f, .midpointY = (clippedY0 + clippedY1) * 50.0f,
            .width = length * 100.0f, .angleDegrees = Math::atan2(physicalDy, dx) * 57.295779513082320876f};
        return Math::isFinite(result.midpointX) && Math::isFinite(result.midpointY) && Math::isFinite(result.width) && Math::isFinite(result.angleDegrees);
    }

private:
    [[nodiscard]] static bool clip(float p, float q, float& enter, float& leave) noexcept
    {
        if (p == 0.0f)
            return q >= 0.0f;
        const float ratio = q / p;
        if (!Math::isFinite(ratio))
            return false;
        if (p < 0.0f) {
            if (ratio > leave)
                return false;
            if (ratio > enter)
                enter = ratio;
        } else {
            if (ratio < enter)
                return false;
            if (ratio < leave)
                leave = ratio;
        }
        return true;
    }
};
