#pragma once

#include <cstddef>
#include "Utils.h"
#include "Vector.h"

struct Ray {
    Ray(const Vector& src, const Vector& dest) : start(src), delta(dest - src) { isSwept = delta.x || delta.y || delta.z; }
    Vector start;
    float pad;
    Vector delta;
    std::byte pad2[40];
    bool isRay{ true };
    bool isSwept;
};

class TraceFilter {
public:
    virtual bool shouldHitEntity(Entity* entity, int) { return entity != skip; }
    virtual int getTraceType() const { return 0; }
    void* skip;
    char* ignore;
};

struct Trace {
    std::byte pad[44];
    float fraction;
    std::byte pad1[28];
    class Entity* entity;
    int hitbox;
};

class EngineTrace {
public:
    constexpr void traceRay(const Ray& ray, unsigned int mask, TraceFilter& filter, Trace& trace) noexcept
    {
        callVirtualFunction<void(__thiscall*)(void*, const Ray&, unsigned int, TraceFilter& , Trace&)>(this, 5)(this, ray, mask, filter, trace);
    }
};
