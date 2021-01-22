#pragma once

#include <cstddef>

#include "Vector.h"
#include "VirtualMethod.h"

#define CONTENTS_EMPTY 0 // No contents

#define CONTENTS_SOLID 0x1 // an eye is never valid in a solid
#define CONTENTS_WINDOW 0x2 // translucent, but not watery (glass)
#define CONTENTS_AUX 0x4
#define CONTENTS_GRATE 0x8 // alpha-tested "grate" textures.  Bullets/sight pass through, but solids don't
#define CONTENTS_SLIME 0x10
#define CONTENTS_WATER 0x20
#define CONTENTS_BLOCKLOS 0x40 // block AI line of sight
#define CONTENTS_OPAQUE 0x80 // things that cannot be seen through (may be non-solid though)
#define LAST_VISIBLE_CONTENTS CONTENTS_OPAQUE

#define ALL_VISIBLE_CONTENTS (LAST_VISIBLE_CONTENTS | (LAST_VISIBLE_CONTENTS - 1))

#define CONTENTS_TESTFOGVOLUME 0x100
#define CONTENTS_UNUSED 0x200

// unused
// NOTE: If it's visible, grab from the top + update LAST_VISIBLE_CONTENTS
// if not visible, then grab from the bottom.
// CONTENTS_OPAQUE + SURF_NODRAW count as CONTENTS_OPAQUE (shadow-casting toolsblocklight textures)
#define CONTENTS_BLOCKLIGHT 0x400

#define CONTENTS_TEAM1 0x800 // per team contents used to differentiate collisions
#define CONTENTS_TEAM2 0x1000 // between players and objects on different teams

// ignore CONTENTS_OPAQUE on surfaces that have SURF_NODRAW
#define CONTENTS_IGNORE_NODRAW_OPAQUE 0x2000

// hits entities which are MOVETYPE_PUSH (doors, plats, etc.)
#define CONTENTS_MOVEABLE 0x4000

// remaining contents are non-visible, and don't eat brushes
#define CONTENTS_AREAPORTAL 0x8000

#define CONTENTS_PLAYERCLIP 0x10000
#define CONTENTS_MONSTERCLIP 0x20000

// currents can be added to any other contents, and may be mixed
#define CONTENTS_CURRENT_0 0x40000
#define CONTENTS_CURRENT_90 0x80000
#define CONTENTS_CURRENT_180 0x100000
#define CONTENTS_CURRENT_270 0x200000
#define CONTENTS_CURRENT_UP 0x400000
#define CONTENTS_CURRENT_DOWN 0x800000

#define CONTENTS_ORIGIN 0x1000000 // removed before bsping an entity

#define CONTENTS_MONSTER 0x2000000 // should never be on a brush, only in game
#define CONTENTS_DEBRIS 0x4000000
#define CONTENTS_DETAIL 0x8000000 // brushes to be added after vis leafs
#define CONTENTS_TRANSLUCENT 0x10000000 // auto set if any surface has trans
#define CONTENTS_LADDER 0x20000000
#define CONTENTS_HITBOX 0x40000000 // use accurate hitboxes on trace

#define MASK_SHOT (CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_MONSTER | CONTENTS_WINDOW | CONTENTS_DEBRIS | CONTENTS_HITBOX)

// NOTE: These are stored in a short in the engine now.  Don't use more than 16 bits
#define SURF_LIGHT 0x0001 // value will hold the light strength
#define SURF_SKY2D 0x0002 // don't draw, indicates we should skylight + draw 2d sky but not draw the 3D skybox
#define SURF_SKY 0x0004 // don't draw, but add to skybox
#define SURF_WARP 0x0008 // turbulent water warp
#define SURF_TRANS 0x0010
#define SURF_NOPORTAL 0x0020 // the surface can not have a portal placed on it
#define SURF_TRIGGER 0x0040 // FIXME: This is an xbox Hacks to work around elimination of trigger surfaces, which breaks occluders
#define SURF_NODRAW 0x0080 // don't bother referencing the texture

#define SURF_HINT 0x0100 // make a primary bsp splitter

#define SURF_SKIP 0x0200 // completely ignore, allowing non-closed brushes
#define SURF_NOLIGHT 0x0400 // Don't calculate light
#define SURF_BUMPLIGHT 0x0800 // calculate three lightmaps for the surface for bumpmapping
#define SURF_NOSHADOWS 0x1000 // Don't receive shadows
#define SURF_NODECALS 0x2000 // Don't receive decals
#define SURF_NOPAINT SURF_NODECALS // the surface can not have paint placed on it
#define SURF_NOCHOP 0x4000 // Don't subdivide patches on this surface
#define SURF_HITBOX 0x8000 // surface is part of a hitbox

#define MASK_ALL (0xFFFFFFFF)
// everything that is normally solid
#define MASK_SOLID (CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_WINDOW | CONTENTS_MONSTER | CONTENTS_GRATE)
// everything that blocks player movement
#define MASK_PLAYERSOLID (CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_PLAYERCLIP | CONTENTS_WINDOW | CONTENTS_MONSTER | CONTENTS_GRATE)
// blocks npc movement
#define MASK_NPCSOLID (CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_MONSTERCLIP | CONTENTS_WINDOW | CONTENTS_MONSTER | CONTENTS_GRATE)
// blocks fluid movement
#define MASK_NPCFLUID (CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_MONSTERCLIP | CONTENTS_WINDOW | CONTENTS_MONSTER)
// water physics in these contents
#define MASK_WATER (CONTENTS_WATER | CONTENTS_MOVEABLE | CONTENTS_SLIME)
// everything that blocks lighting
#define MASK_OPAQUE (CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_OPAQUE)
// everything that blocks lighting, but with monsters added.
#define MASK_OPAQUE_AND_NPCS (MASK_OPAQUE | CONTENTS_MONSTER)
// everything that blocks line of sight for AI
#define MASK_BLOCKLOS (CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_BLOCKLOS)
// everything that blocks line of sight for AI plus NPCs
#define MASK_BLOCKLOS_AND_NPCS (MASK_BLOCKLOS | CONTENTS_MONSTER)
// everything that blocks line of sight for players
#define MASK_VISIBLE (MASK_OPAQUE | CONTENTS_IGNORE_NODRAW_OPAQUE)
// everything that blocks line of sight for players, but with monsters added.
#define MASK_VISIBLE_AND_NPCS (MASK_OPAQUE_AND_NPCS | CONTENTS_IGNORE_NODRAW_OPAQUE)
// bullets see these as solid
#define MASK_SHOT (CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_MONSTER | CONTENTS_WINDOW | CONTENTS_DEBRIS | CONTENTS_HITBOX)
// bullets see these as solid, except monsters (world+brush only)
#define MASK_SHOT_BRUSHONLY (CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_WINDOW | CONTENTS_DEBRIS)
// non-raycasted weapons see this as solid (includes grates)
#define MASK_SHOT_HULL (CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_MONSTER | CONTENTS_WINDOW | CONTENTS_DEBRIS | CONTENTS_GRATE)
// hits solids (not grates) and passes through everything else
#define MASK_SHOT_PORTAL (CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_WINDOW | CONTENTS_MONSTER)
// everything normally solid, except monsters (world+brush only)
#define MASK_SOLID_BRUSHONLY (CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_WINDOW | CONTENTS_GRATE)
// everything normally solid for player movement, except monsters (world+brush only)
#define MASK_PLAYERSOLID_BRUSHONLY (CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_WINDOW | CONTENTS_PLAYERCLIP | CONTENTS_GRATE)
// everything normally solid for npc movement, except monsters (world+brush only)
#define MASK_NPCSOLID_BRUSHONLY (CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_WINDOW | CONTENTS_MONSTERCLIP | CONTENTS_GRATE)
// just the world, used for route rebuilding
#define MASK_NPCWORLDSTATIC (CONTENTS_SOLID | CONTENTS_WINDOW | CONTENTS_MONSTERCLIP | CONTENTS_GRATE)
// just the world, used for route rebuilding
#define MASK_NPCWORLDSTATIC_FLUID (CONTENTS_SOLID | CONTENTS_WINDOW | CONTENTS_MONSTERCLIP)
// These are things that can split areaportals
#define MASK_SPLITAREAPORTAL (CONTENTS_WATER | CONTENTS_SLIME)

// UNDONE: This is untested, any moving water
#define MASK_CURRENT (CONTENTS_CURRENT_0 | CONTENTS_CURRENT_90 | CONTENTS_CURRENT_180 | CONTENTS_CURRENT_270 | CONTENTS_CURRENT_UP | CONTENTS_CURRENT_DOWN)

// everything that blocks corpse movement
// UNDONE: Not used yet / may be deleted
#define MASK_DEADSOLID (CONTENTS_SOLID | CONTENTS_PLAYERCLIP | CONTENTS_WINDOW | CONTENTS_GRATE)

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

struct Plane
{
	Vector normal;
	float dist;
	unsigned char type;
	unsigned char signbits;
	unsigned char pad[2];
};

struct Trace {
    Vector startpos;
    Vector endpos;
    Plane plane;
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
