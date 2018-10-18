#pragma once

#include "ClientEntity.h"
#include "UtlVector.h"

class GlowObjectDefinition_t {
public:
    GlowObjectDefinition_t() { memset(this, 0, sizeof(*this)); }

    class ClientEntity* m_pEntity;    //0x0000
    union
    {
        Vector m_vGlowColor;           //0x0004
        struct
        {
            float   m_flRed;           //0x0004
            float   m_flGreen;         //0x0008
            float   m_flBlue;          //0x000C
        };
    };
    float   m_flAlpha;                 //0x0010
    uint8_t pad_0014[4];               //0x0014
    float   m_flSomeFloat;             //0x0018
    float   m_flBloomAmount;           //0x001C
    float   m_flAnotherFloat;          //0x0020
    bool    m_bRenderWhenOccluded;     //0x0024
    bool    m_bRenderWhenUnoccluded;   //0x0025
    bool    m_bFullBloomRender;        //0x0026
    uint8_t pad_0027[1];               //0x0027
    int m_nFullBloomStencilTestValue;
    int32_t m_nGlowStyle;              //0x002C
    int32_t m_nSplitScreenSlot;        //0x0030
    int32_t m_nNextFreeSlot;           //0x0034

    bool IsUnused() const { return m_nNextFreeSlot != GlowObjectDefinition_t::ENTRY_IN_USE; }

    static constexpr int END_OF_FREE_LIST = -1;
    static constexpr int ENTRY_IN_USE = -2;
}; //Size: 0x0038 (56)

class GlowObjectManager {
public:
    UtlVector<GlowObjectDefinition_t> m_GlowObjectDefinitions;
    int m_nFirstFreeSlot;
};
