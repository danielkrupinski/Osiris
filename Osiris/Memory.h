#pragma once

#include <string_view>

#include "SDK/Entity.h"
#include "SDK/GlobalVars.h"
#include "SDK/GlowObjectManager.h"
#include "SDK/Input.h"

class Memory final {
public:
    Memory() noexcept;
    std::uintptr_t findPattern(const std::string&, const std::string&) const;

    std::uintptr_t present;
    std::uintptr_t reset;

    class ClientMode* clientMode;
    Input* input;
    GlobalVars* globalVars;
    GlowObjectManager* glowObjectManager;

    bool* disablePostProcessing;

    std::uintptr_t loadSky;
    std::uintptr_t setClanTag;
    int* smokeCount;
    std::uintptr_t cameraThink;
    std::uintptr_t revealRanks;
};

extern Memory memory;
