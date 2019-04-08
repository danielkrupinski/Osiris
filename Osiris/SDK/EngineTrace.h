#pragma once

#include <cstddef>
#include "Utils.h"
#include "Vector.h"

struct Ray {
    Ray(const Vector& src, const Vector& dest) : start(src), delta(dest - src) { isSwept = delta.x || delta.y || delta.z; }
    Vector start{ };
    float pad{ };
    Vector delta{ };
    std::byte pad2[40]{ };
    bool isRay{ true };
    bool isSwept{ };
};

class Entity;

struct TraceFilter {
    TraceFilter(const Entity* entity) : skip{ entity } { }
    virtual bool shouldHitEntity(Entity* entity, int) { return entity != skip; }
    virtual int getTraceType() const { return 0; }
    const void* skip;
};

struct Trace {
    std::byte pad[44];
    float fraction;
    std::byte pad1[20];
    int hitgroup;
    int pad2;
    Entity* entity;
    int hitbox;
};

class EngineTrace {
public:
    constexpr void traceRay(const Ray& ray, unsigned int mask, const TraceFilter& filter, Trace& trace) noexcept
    {
        callVirtualFunction<void(__thiscall*)(void*, const Ray&, unsigned int, const TraceFilter& , Trace&)>(this, 5)(this, ray, mask, filter, trace);
    }
};
