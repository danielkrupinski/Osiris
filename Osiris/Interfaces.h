#pragma once

#include <string>

#include "SDK/Client.h"
#include "SDK/ClientEntityList.h"
#include "SDK/EngineClient.h"

class Interfaces final {
public:
    Interfaces();
    void* find(const std::string&, const std::string&);
    EngineClient* engineClient;
    ClientEntityList* clientEntityList;
    Client* client;
};

extern Interfaces interfaces;
