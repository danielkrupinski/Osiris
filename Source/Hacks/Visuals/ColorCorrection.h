#pragma once

#include <cstdint>

#include <Config/ResetConfigurator.h>
#include <CSGO/ClientMode.h>

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

    void run(csgo::ClientMode* clientMode) const noexcept
    {
        if (!enabled)
            return;

        clientMode->blueColorCorrection = blue;
        clientMode->redColorCorrection = red;
        clientMode->monoColorCorrection = mono;
        clientMode->saturationColorCorrection = saturation;
        clientMode->ghostColorCorrection = ghost;
        clientMode->greenColorCorrection = green;
        clientMode->yellowColorCorrection = yellow;
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
