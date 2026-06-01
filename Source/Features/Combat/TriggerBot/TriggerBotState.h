#pragma once

#include <cstdint>

struct TriggerBotState {
    enum class Phase : std::uint8_t { Idle, Pending, Sleep };
    Phase phase{Phase::Idle};
    float phaseStartTime{0.0f};
};
