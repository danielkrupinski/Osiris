#pragma once

#include <cstring>

#include <CS2/Classes/Color.h>
#include <CS2/Classes/CSceneObject.h>
#include <MemoryPatterns/PatternTypes/ClientPatternTypes.h>
#include <Platform/Macros/IsPlatform.h>

template <typename HookContext>
class SceneObjectAttributes {
public:
    SceneObjectAttributes(HookContext& hookContext, cs2::SceneObjectAttributes* attributes) noexcept
        : hookContext{hookContext}
        , attributes{attributes}
    {
    }

    void setAttributeFloat(unsigned int attributeNameHash, float value) const noexcept
    {
        if (!attributes)
            return;

        float float4[]{value, value, value, value};
#if IS_WIN64()
        hookContext.clientPatternSearchResults().template get<SetSceneObjectAttributeFloat4>()(&attributes->floatAttributes, attributeNameHash, float4);
#elif IS_LINUX()
        double temp[2];
        static_assert(sizeof(float4) == sizeof(temp));
        std::memcpy(&temp, &float4, sizeof(float4));
        hookContext.clientPatternSearchResults().template get<SetSceneObjectAttributeFloat4>()(&attributes->floatAttributes, attributeNameHash, temp[0], temp[1]);
#endif
    }

    void setAttributeColor3(unsigned int attributeNameHash, cs2::Color color) const noexcept
    {
        if (!attributes)
            return;

        float colorFloat[]{color.r() / 255.0f, color.g() / 255.0f, color.b() / 255.0f, 0.0f};
#if IS_WIN64()
        hookContext.clientPatternSearchResults().template get<SetSceneObjectAttributeFloat4>()(&attributes->floatAttributes, attributeNameHash, colorFloat);
#elif IS_LINUX()
        double temp[2];
        static_assert(sizeof(colorFloat) == sizeof(temp));
        std::memcpy(&temp, &colorFloat, sizeof(colorFloat));
        hookContext.clientPatternSearchResults().template get<SetSceneObjectAttributeFloat4>()(&attributes->floatAttributes, attributeNameHash, temp[0], temp[1]);
#endif
    }

private:
    HookContext& hookContext;
    cs2::SceneObjectAttributes* attributes;
};
