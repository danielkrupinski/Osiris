#pragma once

#include "Utils.h"

class EngineTrace {
public:
    constexpr void traceRay(const int& ray, unsigned int mask, void* filter, void* trace) noexcept
    {
        callVirtualFunction<void(__thiscall*)(void*, const int&, unsigned int, void*, void*)>(this, 5)(this, ray, mask, filter, trace);
    }
};
