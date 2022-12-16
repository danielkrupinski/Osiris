#pragma once

#include "Inconstructible.h"
#include "VirtualMethod.h"

namespace csgo
{

namespace pod { struct ConVar; }
namespace pod { struct Cvar; }

class Cvar : public VirtualCallableFromPOD<Cvar, pod::Cvar> {
public:
    VIRTUAL_METHOD(pod::ConVar*, findVar, 15, (const char* name), (name))
};

}
