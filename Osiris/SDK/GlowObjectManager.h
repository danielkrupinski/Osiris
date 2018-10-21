#pragma once

#include "ClientEntity.h"
#include "UtlVector.h"

class GlowObjectDefinition {
public:
    GlowObjectDefinition() { memset(this, 0, sizeof(*this)); }

    ClientEntity* entity;
    union
    {
        Vector glowColor;
        struct
        {
            float   red;
            float   green;
            float   blue;
        };
    };
    float   alpha;
    uint8_t pad[4];
    float   m_flSomeFloat;
    float   bloomAmount;
    float   m_flAnotherFloat;
    bool    renderWhenOccluded;
    bool    renderWhenUnoccluded;
    bool    fullBloomRender;
    uint8_t pad1;
    int fullBloomStencilTestValue;
    int32_t glowStyle;
    int32_t splitScreenSlot;
    int32_t nextFreeSlot;

    constexpr bool isUnused() const noexcept { return nextFreeSlot != ENTRY_IN_USE; }

    static constexpr int END_OF_FREE_LIST = -1;
    static constexpr int ENTRY_IN_USE = -2;
};

class GlowObjectManager {
public:
    UtlVector<GlowObjectDefinition> glowObjectDefinitions;
    int firstFreeSlot;
};
