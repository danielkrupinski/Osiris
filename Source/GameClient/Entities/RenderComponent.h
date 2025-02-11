#pragma once

#include <CS2/Classes/CRenderComponent.h>
#include <GameClient/SceneSystem/SceneObjectUpdaters.h>
#include <MemoryPatterns/PatternTypes/RenderComponentPatternTypes.h>

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
        return hookContext.template make<SceneObjectUpdaters>(hookContext.clientPatternSearchResults().template get<OffsetToSceneObjectUpdaters>().of(renderComponent).get());
    }

private:
    HookContext& hookContext;
    cs2::CRenderComponent* renderComponent;
};
