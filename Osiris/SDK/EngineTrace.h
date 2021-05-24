#pragma once

#include <cstddef>

#include "Inconstructible.h"
#include "Vector.h"
#include "VirtualMethod.h"

struct Ray {
    Ray(const Vector& src, const Vector& dest) : start(src), delta(dest - src) { isSwept = delta.x || delta.y || delta.z; }
    Vector start{ };
    float pad{ };
    Vector delta{ };
#ifdef _WIN32
    std::byte pad2[40]{ };
#elif __linux__
    std::byte pad2[44]{ };
#endif
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

namespace HitGroup {
    enum {
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

    constexpr float getDamageMultiplier(int hitGroup) noexcept
    {
        switch (hitGroup) {
        case Head:
            return 4.0f;
        case Stomach:
            return 1.25f;
        case LeftLeg:
        case RightLeg:
            return 0.75f;
        default:
            return 1.0f;
        }
    }

    constexpr bool isArmored(int hitGroup, bool helmet) noexcept
    {
        switch (hitGroup) {
        case Head:
            return helmet;

        case Chest:
        case Stomach:
        case LeftArm:
        case RightArm:
            return true;
        default:
            return false;
        }
    }
}

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

// #define TRACE_STATS // - enable to see how many rays are cast per frame

#ifdef TRACE_STATS
#include "../Memory.h"
#include "GlobalVars.h"
#endif

class EngineTrace {
public:
    INCONSTRUCTIBLE(EngineTrace)

    VIRTUAL_METHOD(int, getPointContents, 0, (const Vector& absPosition, int contentsMask), (this, std::cref(absPosition), contentsMask, nullptr))
    VIRTUAL_METHOD(void, _traceRay, 5, (const Ray& ray, unsigned int mask, const TraceFilter& filter, Trace& trace), (this, std::cref(ray), mask, std::cref(filter), std::ref(trace)))

    void traceRay(const Ray& ray, unsigned int mask, const TraceFilter& filter, Trace& trace) noexcept
    {
#ifdef TRACE_STATS
        static int tracesThisFrame, lastFrame;

        if (lastFrame != memory->globalVars->framecount) {
            memory->debugMsg("traces: frame - %d | count - %d\n", lastFrame, tracesThisFrame);
            tracesThisFrame = 0;
            lastFrame = memory->globalVars->framecount;
        }

        ++tracesThisFrame;
#endif
        _traceRay(ray, mask, filter, trace);
    }
};
