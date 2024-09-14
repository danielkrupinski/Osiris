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

    void apply(auto&& sceneObject, cs2::Color color, int glowRange = 0) const noexcept
    {
        context.applyGlow(sceneObject, color, glowRange);
    }

    void setGlowEntity(auto&& entity) const noexcept
    {
        context.glowEntity() = entity;
    }

    [[nodiscard]] decltype(auto) getAttachedSceneObject() const noexcept
    {
        return context.attachedSceneObject().valueOr(nullptr);
    }

private:
    Context context;
};

template <typename HookContext>
GlowSceneObject(HookContext&, GlowSceneObjectPointer*) -> GlowSceneObject<GlowSceneObjectContext<HookContext>>;
