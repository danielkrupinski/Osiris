#pragma once

#include "VirtualMethod.h"

namespace csgo
{

struct ConVarPOD;
struct CvarPOD;

struct Cvar : VirtualCallableFromPOD<Cvar, CvarPOD> {
    VIRTUAL_METHOD(ConVarPOD*, findVar, 15, (const char* name), (name))
};

}
