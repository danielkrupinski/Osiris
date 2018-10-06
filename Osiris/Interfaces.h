#pragma once

#include <string>

#include "SDK/engine.h"

class Interfaces final {
public:
    Interfaces();
    void* find(const std::string&, const std::string&);
    CEngine* engine;
};

extern Interfaces interfaces;
