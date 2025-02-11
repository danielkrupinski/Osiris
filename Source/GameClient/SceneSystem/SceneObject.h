#pragma once

#include <CS2/Classes/CSceneObject.h>
#include <MemoryPatterns/PatternTypes/SceneObjectPatternTypes.h>

#include "SceneObjectAttributes.h"
#include "SceneSystem.h"

template <typename HookContext>
class SceneObject {
public:
    SceneObject(HookContext& hookContext, cs2::CSceneObject* sceneObject) noexcept
        : hookContext{hookContext}
        , sceneObject{sceneObject}
    {
    }

    [[nodiscard]] explicit operator bool() const noexcept
    {
        return sceneObject != nullptr;
    }

    [[nodiscard]] operator cs2::CSceneObject*() const noexcept
    {
        return sceneObject;
    }

    [[nodiscard]] auto objectClass() const noexcept
    {
        return hookContext.sceneSystemPatternSearchResults().template get<OffsetToSceneObjectClass>().of(sceneObject).toOptional();
    }

    [[nodiscard]] auto attributes() const noexcept
    {
        hookContext.template make<SceneSystem>().allocateAttributeList(sceneObject);
        return hookContext.template make<SceneObjectAttributes>(hookContext.sceneSystemPatternSearchResults().template get<OffsetToSceneObjectAttributes>().of(sceneObject).valueOr(nullptr));
    }

    [[nodiscard]] auto isCulledByFirstPersonView() const noexcept
    {
        // set when camera is inside the upper body e.g. when spectating in first person
        return (renderableFlags() & cs2::SCENEOBJECTFLAG_PIPELINE_SPECIFIC_2).notEqual(0);
    }

private:
    [[nodiscard]] auto renderableFlags() const noexcept
    {
        return hookContext.sceneSystemPatternSearchResults().template get<OffsetToSceneObjectRenderableFlags>().of(sceneObject).toOptional();
    }

    HookContext& hookContext;
    cs2::CSceneObject* sceneObject;
};
