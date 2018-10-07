#pragma once
#include <string>

class Memory final {
public:
    Memory();
    std::uintptr_t findPattern(std::string, std::string);

    std::uintptr_t present;
    std::uintptr_t reset;
    std::uintptr_t localPlayer;
};

extern Memory memory;
