#pragma once
#include <string>

class Memory final {
public:
    Memory();
    std::uintptr_t findPattern(std::string, std::string);

    std::uintptr_t present;
    std::uintptr_t reset;
    std::uintptr_t localPlayer;
    std::uintptr_t globalVars;


    std::uintptr_t isReloading;
    std::uintptr_t isReloading2;
    std::uintptr_t currentCommand;
    std::uintptr_t currentCommand2;
};

extern Memory memory;
