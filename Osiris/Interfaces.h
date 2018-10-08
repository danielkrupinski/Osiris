#pragma once

#include <string>

#include "SDK/GlobalVars.h"
#include "SDK/IBaseClientDll.h"
#include "SDK/IClientEntityList.h"
#include "SDK/IVEngineClient.h"
#include "SDK/Structs.h"
#include "SDK/IVModelInfoClient.h"
#include "SDK/IEngineTrace.h"

class Interfaces final {
public:
    Interfaces();
    void* find(const std::string&, const std::string&);
    IVEngineClient* engine;
    IBaseClientDLL* baseClient;
    CGlobalVarsBase* globalVars;
    C_LocalPlayer localPlayer;
    IClientEntityList* entityList;
    IVModelInfoClient* modelInfo;
    IEngineTrace* engineTrace;
};

extern Interfaces interfaces;
