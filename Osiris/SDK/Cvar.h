#pragma once

#include "VirtualMethod.h"

struct ConVar;

class Cvar {
public:
    VIRTUAL_METHOD(ConVar*, findVar, 15, (const char* name), (this, name))
};
