#pragma once

#include <string_view>

#include "SDK/Entity.h"
#include "SDK/GlowObjectManager.h"
#include "SDK/Input.h"

class Memory final {
public:
    Memory() noexcept;
    std::uintptr_t findPattern(const std::string&, const std::string&) const;

    std::uintptr_t present;
    std::uintptr_t reset;

    Entity** localPlayer;
    class ClientMode* clientMode;
    Input* input;
    GlowObjectManager* glowObjectManager;
    bool* disablePostProcessing;

    std::uintptr_t loadSky;
    std::uintptr_t setClanTag;
    int* smokeCount;
};

extern Memory memory;
