#pragma once

#include <string>

#include "SDK/Entity.h"
#include "SDK/GlobalVars.h"
#include "SDK/GlowObjectManager.h"
#include "SDK/Input.h"

class Memory final {
public:
    Memory() noexcept;
    uintptr_t findPattern(const std::string&, const std::string&) const;

    uintptr_t present;
    uintptr_t reset;

    class ClientMode* clientMode;
    Input* input;
    GlobalVars* globalVars;
    GlowObjectManager* glowObjectManager;

    bool* disablePostProcessing;

    uintptr_t loadSky;
    uintptr_t setClanTag;
    int* smokeCount;
    uintptr_t cameraThink;
    uintptr_t revealRanks;
};

extern Memory memory;
