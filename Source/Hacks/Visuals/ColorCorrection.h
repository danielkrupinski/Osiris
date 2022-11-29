#pragma once

#include <cstdint>

#include <Config/ResetConfigurator.h>
#include <Platform/PlatformSpecific.h>

class ClientMode;

struct ColorCorrection {
    bool enabled;
    float blue;
    float red;
    float mono;
    float saturation;
    float ghost;
    float green;
    float yellow;

    ColorCorrection()
    {
        ResetConfigurator configurator;
        configure(configurator);
    }

    void run(ClientMode* clientMode) const noexcept
    {
        if (!enabled)
            return;

        *reinterpret_cast<float*>(std::uintptr_t(clientMode) + WIN32_LINUX(0x49C, 0x908)) = blue;
        *reinterpret_cast<float*>(std::uintptr_t(clientMode) + WIN32_LINUX(0x4A4, 0x918)) = red;
        *reinterpret_cast<float*>(std::uintptr_t(clientMode) + WIN32_LINUX(0x4AC, 0x928)) = mono;
        *reinterpret_cast<float*>(std::uintptr_t(clientMode) + WIN32_LINUX(0x4B4, 0x938)) = saturation;
        *reinterpret_cast<float*>(std::uintptr_t(clientMode) + WIN32_LINUX(0x4C4, 0x958)) = ghost;
        *reinterpret_cast<float*>(std::uintptr_t(clientMode) + WIN32_LINUX(0x4CC, 0x968)) = green;
        *reinterpret_cast<float*>(std::uintptr_t(clientMode) + WIN32_LINUX(0x4D4, 0x978)) = yellow;
    }

    template <typename Configurator>
    void configure(Configurator& configurator)
    {
        configurator("Enabled", enabled).def(false);
        configurator("Blue", blue).def(0.0f);
        configurator("Red", red).def(0.0f);
        configurator("Mono", mono).def(0.0f);
        configurator("Saturation", saturation).def(0.0f);
        configurator("Ghost", ghost).def(0.0f);
        configurator("Green", green).def(0.0f);
        configurator("Yellow", yellow).def(0.0f);
    }
};
