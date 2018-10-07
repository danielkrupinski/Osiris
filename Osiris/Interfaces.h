#pragma once

#include <string>

#include "SDK/IBaseClientDll.h"
#include "SDK/IVEngineClient.h"

class Interfaces final {
public:
    Interfaces();
    void* find(const std::string&, const std::string&);
    IVEngineClient* engine;
    IBaseClientDLL* baseClient;
};

extern Interfaces interfaces;
