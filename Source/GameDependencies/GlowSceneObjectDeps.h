#pragma once

#include <GameClasses/OffsetTypes/GlowSceneObjectOffset.h>

struct GlowSceneObjectDeps {
    GlowSceneObjectDeps() = default;

    template <typename GlowSceneObjectPatterns>
    explicit GlowSceneObjectDeps(const GlowSceneObjectPatterns& glowSceneObjectPatterns) noexcept
        : offsetToGlowSceneObjectEntity{glowSceneObjectPatterns.offsetToGlowSceneObjectEntity()}
        , offsetToGlowSceneObjectAttachedSceneObject{glowSceneObjectPatterns.offsetToGlowSceneObjectAttachedSceneObject()}
    {
    }

    OffsetToGlowSceneObjectEntity offsetToGlowSceneObjectEntity;
    OffsetToGlowSceneObjectAttachedSceneObject offsetToGlowSceneObjectAttachedSceneObject;
    cs2::CSceneObject::m_nObjectClass glowSceneObjectClass{0xFF};
};
