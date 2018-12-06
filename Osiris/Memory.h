#pragma once

#include <regex>
#include <string_view>

#include "SDK/BaseEntity.h"
#include "SDK/ClientMode.h"
#include "SDK/GlowObjectManager.h"
#include "SDK/Input.h"

class Memory final {
public:
    Memory() noexcept;
    std::uintptr_t findPattern(const std::string_view, const std::string_view) const;
    std::uintptr_t findPattern_2(const std::string&, const std::string&) const;

    std::uintptr_t present;
    std::uintptr_t reset;

    BaseEntity** localPlayer;
    ClientMode* clientMode;
    Input* input;
    GlowObjectManager* glowObjectManager;
    bool* disablePostProcessing;

    std::uintptr_t loadSky;
    std::uintptr_t setClanTag;
    int* smokeCount;
};

extern Memory memory;
