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

enum class HitGroup {
    Invalid = -1,
    Generic,
    Head,
    Chest,
    Stomach,
    LeftArm,
    RightArm,
    LeftLeg,
    RightLeg,
    Gear = 10
};

struct Trace {
    Vector startpos;
    Vector endpos;
    std::byte pad[20];
    float fraction;
    int contents;
    unsigned short dispFlags;
    bool allSolid;
    bool startSolid;
    std::byte pad1[4];
    struct Surface {
        const char* name;
        short surfaceProps;
        unsigned short flags;
    } surface;
    int hitgroup;
    std::byte pad2[4];
    Entity* entity;
    int hitbox;
};

class EngineTrace {
public:
    constexpr auto getPointContents(const Vector& absPosition, int contentsMask) noexcept
    {
        return callVirtualMethod<int, const Vector&, int, Entity*>(this, 0, absPosition, contentsMask, nullptr);
    }

    constexpr void traceRay(const Ray& ray, unsigned int mask, const TraceFilter& filter, Trace& trace) noexcept
    {
        callVirtualMethod<void, const Ray&, unsigned int, const TraceFilter& , Trace&>(this, 5, ray, mask, filter, trace);
    }
};
