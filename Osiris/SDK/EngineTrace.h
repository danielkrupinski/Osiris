#pragma once

#include <cstddef>
#include "Utils.h"
#include "Vector.h"

struct Ray {
    Vector start;
    float pad;
    Vector delta;
    std::byte pad2[37];
    bool isRay;
    bool isSwept;
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
        callVirtualFunction<void(__thiscall*)(void*, const Ray&, unsigned int, TraceFilter& , Trace&)>(this, 5)(this, ray, mask, filter, trace);
    }
};
