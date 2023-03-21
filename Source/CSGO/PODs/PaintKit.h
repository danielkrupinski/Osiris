#pragma once

#include <CSGO/UtlString.h>
#include <Utils/Pad.h>

namespace csgo
{

struct PaintKit {
    int id;
    UtlString name;
    UtlString description;
    UtlString itemName;
    UtlString sameNameFamilyAggregate;
    UtlString pattern;
    UtlString normal;
    UtlString logoMaterial;
    bool baseDiffuseOverride;
    int rarity;
    PAD(40);
    float wearRemapMin;
    float wearRemapMax;
};

}
