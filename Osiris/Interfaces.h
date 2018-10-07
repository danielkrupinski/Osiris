#pragma once

#include <string>

#include "SDK/interfaces/IVEngineClient.hpp"
#include "SDK/interfaces/IClientEntityList.hpp"

class Interfaces final {
public:
    Interfaces();
    void* find(const std::string&, const std::string&);
    IVEngineClient* engine;
    IClientEntityList* entityList;
};

extern Interfaces interfaces;
