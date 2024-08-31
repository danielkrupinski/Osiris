#pragma once

#include <GameClasses/OffsetTypes/RenderComponentOffset.h>

struct RenderComponentDeps {
    template <typename RenderComponentPatterns>
    explicit RenderComponentDeps(const RenderComponentPatterns& renderComponentPatterns) noexcept
        : offsetToSceneObjectUpdaters{renderComponentPatterns.offsetToSceneObjectUpdaters()}
    {
    }

    OffsetToSceneObjectUpdaters offsetToSceneObjectUpdaters;
};
