#pragma once

#include <cstdint>
#include <utility>

#include "SkyChangerState.h"
#include "SkyTextureMapping.h"

template <typename HookContext>
class SkyChanger {
public:
    explicit SkyChanger(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    void setSkyEnabled(bool enabled) noexcept {
        state().enabled = enabled;
    }

    void setSkyType(SkyType skyType) noexcept {
        if (skyType < SkyType::COUNT) {
            state().currentSky = skyType;
        }
    }

    [[nodiscard]] SkyType getCurrentSky() const noexcept {
        return state().currentSky;
    }

    [[nodiscard]] bool isEnabled() const noexcept {
        return state().enabled;
    }

    [[nodiscard]] std::string_view getCurrentSkyTexture() const noexcept {
        return SkyTextureMapping::getSkyTexture(state().currentSky);
    }

    [[nodiscard]] std::string_view getCurrentSkyName() const noexcept {
        return SkyTextureMapping::getSkyName(state().currentSky);
    }

    void applySkyTexture() const noexcept {
        if (!state().enabled)
            return;

        // This function would be called from the render hook
        // Texture application implementation depends on CS2 internals
        // For now, this serves as the interface
    }

private:
    [[nodiscard]] SkyChangerState& state() noexcept {
        return hookContext.skyChangerState();
    }

    [[nodiscard]] const SkyChangerState& state() const noexcept {
        return hookContext.skyChangerState();
    }

    HookContext& hookContext;
};
