#pragma once

#include <GameClasses/OffsetTypes/GlowSceneObjectOffset.h>

struct GlowSceneObjectDeps {
    template <typename GlowSceneObjectPatterns>
    explicit GlowSceneObjectDeps(const GlowSceneObjectPatterns& glowSceneObjectPatterns) noexcept
        : offsetToGlowSceneObjectEntity{glowSceneObjectPatterns.offsetToGlowSceneObjectEntity()}
        , offsetToSceneObjectFlags{glowSceneObjectPatterns.offsetToSceneObjectFlags()}
    {
    }

    OffsetToGlowSceneObjectEntity offsetToGlowSceneObjectEntity;
    OffsetToSceneObjectFlags offsetToSceneObjectFlags;
};
