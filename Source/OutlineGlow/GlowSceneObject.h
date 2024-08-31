#pragma once

#include <utility>

#include "GlowSceneObjectContext.h"

template <typename Context>
class GlowSceneObject {
public:
    template <typename... Args>
    GlowSceneObject(Args&&... args) noexcept
        : context{std::forward<Args>(args)...}
    {
    }

    void apply(auto&& entity, cs2::Color color) const noexcept
    {
        context.applyGlow(entitySceneObject(entity), color);
        context.setGlowEntity(entity);
    }

private:
    [[nodiscard]] decltype(auto) entitySceneObject(auto&& entity) const noexcept
    {
        return entity.renderComponent().sceneObjectUpdaters()[0].sceneObject();
    }

    Context context;
};

template <typename HookContext>
GlowSceneObject(HookContext&, GlowSceneObjectPointer*) -> GlowSceneObject<GlowSceneObjectContext<HookContext>>;
