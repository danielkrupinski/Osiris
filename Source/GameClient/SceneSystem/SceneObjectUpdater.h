#pragma once

#include <CS2/Classes/SceneObjectUpdaterHandle_t.h>
#include <MemoryPatterns/PatternTypes/SceneObjectUpdaterPatternTypes.h>

#include "SceneObject.h"

template <typename HookContext>
class SceneObjectUpdater {
public:
    SceneObjectUpdater(HookContext& hookContext, cs2::SceneObjectUpdaterHandle_t* updater) noexcept
        : hookContext{hookContext}
        , updater{updater}
    {
    }

    [[nodiscard]] decltype(auto) sceneObject() const noexcept
    {
        return hookContext.template make<SceneObject>(hookContext.clientPatternSearchResults().template get<OffsetToSceneObject>().of(updater).valueOr(nullptr));
    }

private:
    HookContext& hookContext;
    cs2::SceneObjectUpdaterHandle_t* updater;
};
