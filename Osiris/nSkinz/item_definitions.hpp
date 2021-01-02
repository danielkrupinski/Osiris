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

enum ItemDefinitionIndex : int {
    WEAPON_KNIFE = 42,
    GLOVE_T_SIDE = 5028
};

//extern const std::map<size_t, weapon_info> k_weapon_info;
namespace game_data
{
    struct weapon_name
    {
        constexpr weapon_name(const int definition_index, const char* name) :
            definition_index(definition_index),
            name(name)
        {}

        int definition_index = 0;
        const char* name = nullptr;
    };

    constexpr auto weapon_names = std::to_array<weapon_name>({
        {WEAPON_KNIFE, "Knife"},
        {GLOVE_T_SIDE, "Glove"},
        {7, "AK-47"},
        {8, "AUG"},
        {9, "AWP"},
        {63, "CZ75 Auto"},
        {1, "Desert Eagle"},
        {2, "Dual Berettas"},
        {10, "FAMAS"},
        {3, "Five-SeveN"},
        {11, "G3SG1"},
        {13, "Galil AR"},
        {4, "Glock-18"},
        {14, "M249"},
        {60, "M4A1-S"},
        {16, "M4A4"},
        {17, "MAC-10"},
        {27, "MAG-7"},
        {23, "MP5-SD"},
        {33, "MP7"},
        {34, "MP9"},
        {28, "Negev"},
        {35, "Nova"},
        {32, "P2000"},
        {36, "P250"},
        {19, "P90"},
        {26, "PP-Bizon"},
        {64, "R8 Revolver"},
        {29, "Sawed-Off"},
        {38, "SCAR-20"},
        {40, "SSG 08"},
        {39, "SG 553"},
        {30, "Tec-9"},
        {24, "UMP-45"},
        {61, "USP-S"},
        {25, "XM1014"},
    });
}
