#pragma once

#include "VirtualMethod.h"

namespace csgo
{

struct ConVarPOD;
struct CvarPOD;

class Cvar : public VirtualCallableFromPOD<Cvar, CvarPOD> {
public:
    VIRTUAL_METHOD(ConVarPOD*, findVar, 15, (const char* name), (name))
};

}
