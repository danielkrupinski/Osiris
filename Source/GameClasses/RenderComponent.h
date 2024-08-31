#pragma once

#include <CS2/Classes/CRenderComponent.h>

#include "SceneObjectUpdaters.h"

template <typename HookContext>
class RenderComponent {
public:
    RenderComponent(HookContext& hookContext, cs2::CRenderComponent* renderComponent) noexcept
        : hookContext{hookContext}
        , renderComponent{renderComponent}
    {
    }

    [[nodiscard]] decltype(auto) sceneObjectUpdaters() const noexcept
    {
        return hookContext.template make<SceneObjectUpdaters>(hookContext.gameDependencies().renderComponentDeps.offsetToSceneObjectUpdaters.of(renderComponent).get());
    }

private:
    HookContext& hookContext;
    cs2::CRenderComponent* renderComponent;
};
