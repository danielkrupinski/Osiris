#pragma once

#include "Utils.h"

struct ConVar;

class Cvar {
public:
    constexpr auto findVar(const char* name) noexcept
    {
        return callVirtualMethod<ConVar*, const char*>(this, 15, name);
    }
};
