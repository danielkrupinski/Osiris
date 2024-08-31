#pragma once

#include "GlowSceneObjectPointer.h"

template <typename HookContext>
class GlowSceneObjectContext {
public:
    GlowSceneObjectContext(HookContext& hookContext, GlowSceneObjectPointer* glowSceneObjectPointer) noexcept
        : hookContext{hookContext}
        , glowSceneObjectPointer{glowSceneObjectPointer}
    {
    }

    void applyGlow(auto&& sceneObject, cs2::Color color) const noexcept
    {
        if (!sceneObject || !glowSceneObjectPointer)
            return;

        if (const auto manageGlowSceneObject = hookContext.gameDependencies().manageGlowSceneObject) {
            cs2::CGlowHelperSceneObject* glowSceneObject{glowSceneObjectPointer->value()};
            cs2::CGlowHelperSceneObject* dummy{nullptr};
            float colorFloat[4]{color.r() / 255.0f, color.g() / 255.0f, color.b() / 255.0f, color.a() / 255.0f};

#if IS_WIN64()
            manageGlowSceneObject(&glowSceneObject, &dummy, sceneObject, colorFloat, 0, 0, 3, 1.0f);
#elif IS_LINUX()
            double colorDouble[2];
            static_assert(sizeof(colorFloat) == sizeof(colorDouble));
            std::memcpy(colorDouble, colorFloat, sizeof(colorFloat));
            manageGlowSceneObject(&glowSceneObject, &dummy, sceneObject, 3, colorDouble[0], colorDouble[1], 0.0f, 0.0f, 1.0f);
#endif

            glowSceneObjectPointer->setValue(glowSceneObject);
        }
    }

    void setGlowEntity(auto&& entity) const noexcept
    {
        if (glowSceneObjectPointer && glowSceneObjectPointer->value()) {
            hookContext.gameDependencies().glowSceneObjectDeps.offsetToGlowSceneObjectEntity.of(glowSceneObjectPointer->value()) = static_cast<cs2::C_BaseEntity*>(entity);
        }
    }

private:
    HookContext& hookContext;
    GlowSceneObjectPointer* glowSceneObjectPointer;
};

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
