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
    uintptr_t findPattern_2(const char*, std::string_view) const;

    uintptr_t present;
    uintptr_t reset;

    class ClientMode* clientMode;
    Input* input;
    GlobalVars* globalVars;
    GlowObjectManager* glowObjectManager;

    bool* disablePostProcessing;

    uintptr_t loadSky;
    std::add_pointer_t<void __fastcall(const char*, const char*) noexcept> setClanTag;
    int* smokeCount;
    uintptr_t cameraThink;
    std::add_pointer_t<void __cdecl(int*) noexcept> revealRanks;
    std::add_pointer_t<bool __stdcall(const char*) noexcept> acceptMatch;
};

extern Memory memory;
