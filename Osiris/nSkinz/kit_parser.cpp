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
#include "kit_parser.hpp"
#include "Utilities/platform.hpp"
#include "../SDK/Localize.h"
#include "../SDK/ItemSchema.h"
#include "../Interfaces.h"
#include "../Memory.h"

#include <algorithm>

std::vector<game_data::PaintKit> game_data::skin_kits;
std::vector<game_data::PaintKit> game_data::glove_kits;
std::vector<game_data::PaintKit> game_data::sticker_kits;

void game_data::initialize_kits() noexcept
{
    const auto V_UCS2ToUTF8 = static_cast<int(*)(const wchar_t* ucs2, char* utf8, int len)>(platform::get_export("vstdlib.dll", "V_UCS2ToUTF8"));

    for (int i = 0; i <= memory.itemSchema()->paintKits.lastElement; i++) {
        const auto paint_kit = memory.itemSchema()->paintKits.memory[i].value;

        if (paint_kit->id == 9001)
            continue;

        const auto wide_name = interfaces.localize->find(paint_kit->itemName.buffer + 1);
        char name[256];
        V_UCS2ToUTF8(wide_name, name, sizeof(name));

        if (paint_kit->id < 10000)
            game_data::skin_kits.emplace_back(paint_kit->id, name);
        else
            game_data::glove_kits.emplace_back(paint_kit->id, name);
    }

    std::sort(game_data::skin_kits.begin(), game_data::skin_kits.end());
    std::sort(game_data::glove_kits.begin(), game_data::glove_kits.end());


    game_data::sticker_kits.emplace_back(0, "None");

    for (int i = 0; i <= memory.itemSchema()->stickerKits.lastElement; i++) {
        const auto sticker_kit = memory.itemSchema()->stickerKits.memory[i].value;

        char sticker_name_if_valve_fucked_up_their_translations[64];

        auto sticker_name_ptr = sticker_kit->itemName.buffer + 1;

        if (strstr(sticker_name_ptr, "StickerKit_dhw2014_dignitas")) {
            strcpy_s(sticker_name_if_valve_fucked_up_their_translations, "StickerKit_dhw2014_teamdignitas");
            strcat_s(sticker_name_if_valve_fucked_up_their_translations, sticker_name_ptr + 27);
            sticker_name_ptr = sticker_name_if_valve_fucked_up_their_translations;
        }

        const auto wide_name = interfaces.localize->find(sticker_name_ptr);
        char name[256];
        V_UCS2ToUTF8(wide_name, name, sizeof(name));

        game_data::sticker_kits.emplace_back(sticker_kit->id, name);
    }
    std::sort(std::next(game_data::sticker_kits.begin()), game_data::sticker_kits.end());
}
