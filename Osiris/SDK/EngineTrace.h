#pragma once

#include <cstddef>
#include "Utils.h"


struct Ray {
    std::byte pad[80];
};

class TraceFilter { };

struct Trace {
    std::byte pad[76];
    class Entity* entity;
};

class EngineTrace {
public:
    constexpr void traceRay(const Ray& ray, unsigned int mask, TraceFilter& filter, Trace& trace) noexcept
    {
        callVirtualFunction<void(__thiscall*)(void*, const Ray&, unsigned int, TraceFilter& , Trace&) > (this, 5)(this, ray, mask, filter, trace);
    }
};
