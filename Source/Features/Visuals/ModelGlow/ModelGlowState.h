#pragma once

#include <cstdint>
#include <CS2/Classes/Entities/C_CSPlayerPawn.h>

struct ModelGlowState {
    enum class State : std::uint8_t {
        Enabled,
        Disabling,
        Disabled
    };

    State masterSwitch{State::Disabled};
    State playerModelGlow{State::Enabled};
    bool showOnlyEnemies{true};

    std::uint64_t(*originalPlayerPawnSceneObjectUpdater)(cs2::C_CSPlayerPawn* playerPawn, void*, bool){nullptr};
};
