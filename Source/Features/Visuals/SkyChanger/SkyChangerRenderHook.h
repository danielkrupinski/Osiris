#pragma once

#include <cstdint>
#include "SkyChangerImpl.h"

template <typename HookContext>
class SkyChangerRenderHook {
public:
    explicit SkyChangerRenderHook(HookContext& hookContext) noexcept
        : hookContext{hookContext}, skyChangerImpl{hookContext}
    {
    }

    void onRenderStart() noexcept {
        // Called every frame from ViewRenderHook
        // Apply sky texture changes if enabled
        skyChangerImpl.applySkyTexture();
    }

    void setSkyEnabled(bool enabled) noexcept {
        skyChangerImpl.setSkyEnabled(enabled);
    }

    void setSkyType(SkyType skyType) noexcept {
        skyChangerImpl.setSkyType(skyType);
    }

    [[nodiscard]] bool isEnabled() const noexcept {
        return skyChangerImpl.isEnabled();
    }

    [[nodiscard]] SkyType getCurrentSky() const noexcept {
        return skyChangerImpl.getCurrentSky();
    }

    [[nodiscard]] std::string_view getCurrentSkyName() const noexcept {
        return skyChangerImpl.getCurrentSkyName();
    }

private:
    HookContext& hookContext;
    SkyChangerImpl<HookContext> skyChangerImpl;
};