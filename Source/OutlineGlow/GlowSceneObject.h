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

    [[nodiscard]] decltype(auto) baseSceneObject() const noexcept
    {
        return context.baseSceneObject();
    }

    void apply(auto&& sceneObject, cs2::Color color, int glowRange = 0) const noexcept
    {
        context.applyGlow(sceneObject, color, glowRange);
        storeGlowSceneObjectClass();
    }

    void setGlowEntity(auto&& entity) const noexcept
    {
        context.glowEntity() = entity;
    }

    [[nodiscard]] decltype(auto) getAttachedSceneObject() const noexcept
    {
        return context.attachedSceneObject().valueOr(nullptr);
    }

    [[nodiscard]] auto isValidGlowSceneObject() const noexcept
    {
        return baseSceneObject().objectClass().equal(context.storedGlowSceneObjectClass());
    }

private:
    void storeGlowSceneObjectClass() const noexcept
    {
        if (auto& storedGlowSceneObjectClass = context.storedGlowSceneObjectClass(); storedGlowSceneObjectClass == 0xFF) {
            if (const auto objectClass = baseSceneObject().objectClass(); objectClass.hasValue())
                storedGlowSceneObjectClass = objectClass.value();
        }
    }

    Context context;
};

template <typename HookContext>
GlowSceneObject(HookContext&, GlowSceneObjectPointer*) -> GlowSceneObject<GlowSceneObjectContext<HookContext>>;
