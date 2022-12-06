#pragma once

#include "Inconstructible.h"
#include "VirtualMethod.h"

namespace csgo::pod
{
    struct ConVar;
    struct Cvar;
}

class Cvar : public VirtualCallableFromPOD<Cvar, csgo::pod::Cvar> {
public:
    VIRTUAL_METHOD(csgo::pod::ConVar*, findVar, 15, (const char* name), (name))
};
