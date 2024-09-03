#pragma once

#include <GameClasses/OffsetTypes/GlowSceneObjectOffset.h>

struct GlowSceneObjectDeps {
    template <typename GlowSceneObjectPatterns>
    explicit GlowSceneObjectDeps(const GlowSceneObjectPatterns& glowSceneObjectPatterns) noexcept
        : offsetToGlowSceneObjectEntity{glowSceneObjectPatterns.offsetToGlowSceneObjectEntity()}
        , offsetToSceneObjectFlags{glowSceneObjectPatterns.offsetToSceneObjectFlags()}
        , offsetToGlowSceneObjectAttachedSceneObject{glowSceneObjectPatterns.offsetToGlowSceneObjectAttachedSceneObject()}
    {
    }

    OffsetToGlowSceneObjectEntity offsetToGlowSceneObjectEntity;
    OffsetToSceneObjectFlags offsetToSceneObjectFlags;
    OffsetToGlowSceneObjectAttachedSceneObject offsetToGlowSceneObjectAttachedSceneObject;
};
