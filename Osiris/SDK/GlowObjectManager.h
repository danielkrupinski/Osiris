#pragma once

#include "ClientEntity.h"
#include "UtlVector.h"

class GlowObjectDefinition {
public:
    GlowObjectDefinition() { memset(this, 0, sizeof(*this)); }

    class ClientEntity* m_pEntity;
    union
    {
        Vector m_vGlowColor;
        struct
        {
            float   m_flRed;
            float   m_flGreen;
            float   m_flBlue;
        };
    };
    float   m_flAlpha;
    uint8_t pad_0014[4];
    float   m_flSomeFloat;
    float   m_flBloomAmount;
    float   m_flAnotherFloat;
    bool    m_bRenderWhenOccluded;
    bool    m_bRenderWhenUnoccluded;
    bool    m_bFullBloomRender;
    uint8_t pad_0027[1];
    int m_nFullBloomStencilTestValue;
    int32_t m_nGlowStyle;
    int32_t m_nSplitScreenSlot;
    int32_t m_nNextFreeSlot;

    constexpr bool isUnused() const noexcept { return m_nNextFreeSlot != GlowObjectDefinition::ENTRY_IN_USE; }

    static constexpr int END_OF_FREE_LIST = -1;
    static constexpr int ENTRY_IN_USE = -2;
};

class GlowObjectManager {
public:
    UtlVector<GlowObjectDefinition> m_GlowObjectDefinitions;
    int m_nFirstFreeSlot;
};
