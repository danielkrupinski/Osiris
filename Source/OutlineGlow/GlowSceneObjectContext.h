#pragma once

#include <cstring>

#include <CS2/Classes/Color.h>
#include <CS2/Classes/Glow.h>
#include <GameClient/SceneSystem/SceneObject.h>
#include <MemoryPatterns/PatternTypes/ClientPatternTypes.h>
#include <MemoryPatterns/PatternTypes/GlowSceneObjectPatternTypes.h>
#include <Platform/Macros/IsPlatform.h>

#include "GlowSceneObjectPointer.h"

template <typename HookContext>
class GlowSceneObjectContext {
public:
    GlowSceneObjectContext(HookContext& hookContext, GlowSceneObjectPointer* glowSceneObjectPointer) noexcept
        : hookContext{hookContext}
        , glowSceneObjectPointer{glowSceneObjectPointer}
    {
    }

    [[nodiscard]] decltype(auto) baseSceneObject() const noexcept
    {
        return hookContext.template make<SceneObject>(glowSceneObject());
    }

    void applyGlow(auto&& sceneObject, cs2::Color color, int glowRange) const noexcept
    {
        if (!sceneObject || !glowSceneObjectPointer)
            return;

        if (const auto manageGlowSceneObject = hookContext.clientPatternSearchResults().template get<ManageGlowSceneObjectPointer>()) {
            cs2::CGlowHelperSceneObject* tempGlowSceneObject{glowSceneObject()};
            cs2::CGlowHelperSceneObject* dummy{nullptr};
            float colorFloat[4]{color.r() / 255.0f, color.g() / 255.0f, color.b() / 255.0f, color.a() / 255.0f};

#if IS_WIN64()
            manageGlowSceneObject(&tempGlowSceneObject, &dummy, sceneObject, colorFloat, 0, static_cast<float>(glowRange), 3, 1.0f);
#elif IS_LINUX()
            double colorDouble[2];
            static_assert(sizeof(colorFloat) == sizeof(colorDouble));
            std::memcpy(colorDouble, colorFloat, sizeof(colorFloat));
            manageGlowSceneObject(&tempGlowSceneObject, &dummy, sceneObject, 3, colorDouble[0], colorDouble[1], 0.0f, static_cast<float>(glowRange), 1.0f);
#endif
            glowSceneObjectPointer->setValue(tempGlowSceneObject);
        }
    }

    [[nodiscard]] decltype(auto) glowEntity() const noexcept
    {
        return hookContext.clientPatternSearchResults().template get<OffsetToGlowSceneObjectEntity>().of(glowSceneObject());
    }

    [[nodiscard]] decltype(auto) attachedSceneObject() const noexcept
    {
        return hookContext.clientPatternSearchResults().template get<OffsetToGlowSceneObjectAttachedSceneObject>().of(glowSceneObject());
    }

    [[nodiscard]] auto& storedGlowSceneObjectClass() const noexcept
    {
        return hookContext.glowSceneObjectState().glowSceneObjectClass;
    }

private:
    [[nodiscard]] cs2::CGlowHelperSceneObject* glowSceneObject() const noexcept
    {
        if (glowSceneObjectPointer)
            return glowSceneObjectPointer->value();
        return nullptr;
    }

    HookContext& hookContext;
    GlowSceneObjectPointer* glowSceneObjectPointer;
};
