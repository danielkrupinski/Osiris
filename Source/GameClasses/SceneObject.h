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

    [[nodiscard]] auto isDeleted() const noexcept
    {
        return (hookContext.sceneSystemPatternSearchResults().template get<OffsetToSceneObjectFlags>().toOptional() & cs2::SceneObjectFlag_IsDeleted).notEqual(0);
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

private:
    HookContext& hookContext;
    cs2::CSceneObject* sceneObject;
};
