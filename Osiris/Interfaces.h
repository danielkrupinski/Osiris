#pragma once

#include <string>

#include "SDK/GlobalVars.h"

class Interfaces final {
public:
    Interfaces();
    void* find(const std::string&, const std::string&);

    GlobalVars* globalVars;
};

extern Interfaces interfaces;
