#pragma once

#include <cstdint>

#include <CSGO/Constants/ItemId.h>
#include <CSGO/WeaponId.h>
#include <Utils/Pad.h>

namespace csgo
{

struct EconItemPOD {
    PAD(2 * sizeof(std::uintptr_t));

    ItemId itemID;
    ItemId originalID;
    void* customDataOptimizedObject;
    std::uint32_t accountID;
    std::uint32_t inventory;
    WeaponId weaponId;

    std::uint16_t origin : 5;
    std::uint16_t quality : 4;
    std::uint16_t level : 2;
    std::uint16_t rarity : 4;
    std::uint16_t dirtybitInUse : 1;

    std::int16_t itemSet;
    int soUpdateFrame;
    std::uint8_t flags;
};

}
