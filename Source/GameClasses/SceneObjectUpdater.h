#pragma once

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
        return hookContext.clientPatternSearchResults().template get<OffsetToSceneObject>().of(updater).valueOr(nullptr);
    }

private:
    HookContext& hookContext;
    cs2::SceneObjectUpdaterHandle_t* updater;
};
