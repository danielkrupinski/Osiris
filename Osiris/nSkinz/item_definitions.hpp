/* This file is part of nSkinz by namazso, licensed under the MIT license:
*
* MIT License
*
* Copyright (c) namazso 2018
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/
#pragma once

#include <array>
#include <string>

#include "../SDK/WeaponId.h"

enum ItemDefinitionIndex : int {
    WEAPON_KNIFE = 42,
    GLOVE_T_SIDE = 5028
};

//extern const std::map<size_t, weapon_info> k_weapon_info;
namespace game_data
{
    struct weapon_name
    {
        constexpr weapon_name(WeaponId definition_index, const char* name) :
            definition_index(definition_index),
            name(name)
        {}

        WeaponId definition_index;
        const char* name;
    };

    constexpr auto weapon_names = std::to_array<weapon_name>({
        {WeaponId::Knife, "Knife"},
        {WeaponId::GloveT, "Glove"},
        {WeaponId::Ak47, "AK-47"},
        {WeaponId::Aug, "AUG"},
        {WeaponId::Awp, "AWP"},
        {WeaponId::Cz75a, "CZ75 Auto"},
        {WeaponId::Deagle, "Desert Eagle"},
        {WeaponId::Elite, "Dual Berettas"},
        {WeaponId::Famas, "FAMAS"},
        {WeaponId::Fiveseven, "Five-SeveN"},
        {WeaponId::G3SG1, "G3SG1"},
        {WeaponId::GalilAr, "Galil AR"},
        {WeaponId::Glock, "Glock-18"},
        {WeaponId::M249, "M249"},
        {WeaponId::M4a1_s, "M4A1-S"},
        {WeaponId::M4A1, "M4A4"},
        {WeaponId::Mac10, "MAC-10"},
        {WeaponId::Mag7, "MAG-7"},
        {WeaponId::Mp5sd, "MP5-SD"},
        {WeaponId::Mp7, "MP7"},
        {WeaponId::Mp9, "MP9"},
        {WeaponId::Negev, "Negev"},
        {WeaponId::Nova, "Nova"},
        {WeaponId::Hkp2000, "P2000"},
        {WeaponId::P250, "P250"},
        {WeaponId::P90, "P90"},
        {WeaponId::Bizon, "PP-Bizon"},
        {WeaponId::Revolver, "R8 Revolver"},
        {WeaponId::Sawedoff, "Sawed-Off"},
        {WeaponId::Scar20, "SCAR-20"},
        {WeaponId::Ssg08, "SSG 08"},
        {WeaponId::Sg553, "SG 553"},
        {WeaponId::Tec9, "Tec-9"},
        {WeaponId::Ump45, "UMP-45"},
        {WeaponId::Usp_s, "USP-S"},
        {WeaponId::Xm1014, "XM1014"}
    });
}
