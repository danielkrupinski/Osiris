#pragma once

#include "UtlVector.h"
#include "Vector.h"

class Entity;

struct GlowObjectDefinition {
    int nextFreeSlot;
    Entity* entity;

    // TODO: try to declare those as std::array<float, 4> for easier color copying
    Vector glowColor;
    float glowAlpha;
    //

    bool glowAlphaCappedByRenderAlpha;
    float glowAlphaFunctionOfMaxVelocity;
    float glowAlphaMax;
    float glowPulseOverdrive;
    bool renderWhenOccluded;
    bool renderWhenUnoccluded;
    bool fullBloomRender;
    int fullBloomStencilTestValue;
    int glowStyle;
    int splitScreenSlot;

    constexpr bool isUnused() const noexcept { return nextFreeSlot != ENTRY_IN_USE; }

    static constexpr int END_OF_FREE_LIST = -1;
    static constexpr int ENTRY_IN_USE = -2;
};

struct GlowObjectManager {
    UtlVector<GlowObjectDefinition> glowObjectDefinitions;

    constexpr bool hasGlowEffect(Entity* entity) noexcept
    {
        for (int i = 0; i < glowObjectDefinitions.size; i++)
            if (!glowObjectDefinitions[i].isUnused() && glowObjectDefinitions[i].entity == entity)
                return true;

        return false;
    }

    constexpr int registerGlowObject(Entity* entity) noexcept
    {
        int index = firstFreeSlot;
        if (index != -1) {
            firstFreeSlot = glowObjectDefinitions[index].nextFreeSlot;
            glowObjectDefinitions[index].entity = entity;
            glowObjectDefinitions[index].fullBloomRender = false;
            glowObjectDefinitions[index].fullBloomStencilTestValue = 0;
            glowObjectDefinitions[index].splitScreenSlot = -1;
            glowObjectDefinitions[index].nextFreeSlot = -2;
        }
        return index;
    }

    constexpr void unregisterGlowObject(int index) noexcept
    {
        glowObjectDefinitions[index].nextFreeSlot = firstFreeSlot;
        glowObjectDefinitions[index].entity = nullptr;
        glowObjectDefinitions[index].renderWhenOccluded = false;
        glowObjectDefinitions[index].renderWhenUnoccluded = false;
        firstFreeSlot = index;
    }

    int firstFreeSlot;
};
