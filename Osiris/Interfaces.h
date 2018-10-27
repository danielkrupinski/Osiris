#pragma once

#include <string>

#include "SDK/Client.h"
#include "SDK/ClientEntityList.h"
#include "SDK/Cvar.h"
#include "SDK/EngineClient.h"
#include "SDK/Material.h"
#include "SDK/Surface.h"

class Interfaces final {
public:
    Interfaces();

    EngineClient* engineClient;
    ClientEntityList* clientEntityList;
    Client* client;
    Surface* surface;
private:
    void* find(const std::string&, const std::string&);
};

extern Interfaces interfaces;
