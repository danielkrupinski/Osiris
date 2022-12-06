#pragma once

#include <SDK/Pad.h>
#include <SDK/UtlString.h>

namespace csgo::pod
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
    PAD(40)
    float wearRemapMin;
    float wearRemapMax;
};

}
