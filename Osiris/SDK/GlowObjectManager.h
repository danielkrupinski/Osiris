#pragma once

#include "UtlVector.h"
#include "Vector.h"

class Entity;

struct GlowObjectDefinition {
    Entity* entity;
    Vector glowColor;
    float   alpha;
    std::byte pad[4];
    float   m_flSomeFloat;
    float   bloomAmount;
    float   m_flAnotherFloat;
    bool    renderWhenOccluded;
    bool    renderWhenUnoccluded;
    bool    fullBloomRender;
    std::byte pad1;
    int fullBloomStencilTestValue;
    int glowStyle;
    int splitScreenSlot;
    int nextFreeSlot;

    constexpr bool isUnused() const noexcept { return nextFreeSlot != ENTRY_IN_USE; }

    static constexpr int END_OF_FREE_LIST = -1;
    static constexpr int ENTRY_IN_USE = -2;
};

struct GlowObjectManager {
    UtlVector<GlowObjectDefinition> glowObjectDefinitions;
    int firstFreeSlot;
};
