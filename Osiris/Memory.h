#pragma once
#include <string>

#include "SDK/GlobalVars.h"

class Memory final {
public:
    Memory();
    std::uintptr_t findPattern(std::string, std::string);

    std::uintptr_t present;
    std::uintptr_t reset;

    GlobalVars* globalVars;
};

extern Memory memory;
