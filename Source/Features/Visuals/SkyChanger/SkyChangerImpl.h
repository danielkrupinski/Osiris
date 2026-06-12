#pragma once

#include <algorithm>
#include <cstdint>
#include <string_view>
#include <CS2/Classes/CSceneObject.h>
#include <GameClient/SceneSystem/SceneSystem.h>
#include <MemoryAllocation/MemoryAllocator.h>
#include "SkyChanger.h"
#include "SkyTextureMapping.h"

template <typename HookContext>
class SkyChangerImpl {
public:
    explicit SkyChangerImpl(HookContext& hookContext) noexcept
        : hookContext{hookContext}, skyChanger{hookContext}
    {
    }

    void applySkyTexture() noexcept {
        if (!skyChanger.isEnabled())
            return;

        const auto skyType = skyChanger.getCurrentSky();
        if (skyType == SkyType::Default)
            return;

        try {
            const auto texturePath = SkyTextureMapping::getSkyTexture(skyType);
            replaceSkybox(texturePath);
        } catch (...) {
            // Silently fail - sky change is cosmetic
        }
    }

    void setSkyEnabled(bool enabled) noexcept {
        skyChanger.setSkyEnabled(enabled);
    }

    void setSkyType(SkyType skyType) noexcept {
        skyChanger.setSkyType(skyType);
    }

    [[nodiscard]] bool isEnabled() const noexcept {
        return skyChanger.isEnabled();
    }

    [[nodiscard]] SkyType getCurrentSky() const noexcept {
        return skyChanger.getCurrentSky();
    }

    [[nodiscard]] std::string_view getCurrentSkyName() const noexcept {
        return SkyTextureMapping::getSkyName(skyChanger.getCurrentSky());
    }

private:
    void replaceSkybox(std::string_view texturePath) const noexcept {
        // CS2 sky replacement implementation
        // The sky is rendered as a skybox entity in the scene
        // We need to find and modify its material properties
        
        // For a full implementation, this would:
        // 1. Iterate through all scene objects
        // 2. Find the skybox mesh entity
        // 3. Replace its material texture with the new sky texture
        // 4. Update the material cache
        
        // Note: Actual texture replacement depends on CS2 internals
        // The material system may be protected or require specific offsets
    }

    HookContext& hookContext;
    SkyChanger<HookContext> skyChanger;
};