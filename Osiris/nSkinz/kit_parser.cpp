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
#include "../Interfaces.h"
#include "../Memory.h"

#include <algorithm>

std::vector<game_data::PaintKit> game_data::skin_kits;
std::vector<game_data::PaintKit> game_data::glove_kits;
std::vector<game_data::PaintKit> game_data::sticker_kits;

class CCStrike15ItemSchema;
class CCStrike15ItemSystem;

template <typename Key, typename Value>
struct Node_t
{
    int previous_id;		//0x0000
    int next_id;			//0x0004
    void* _unknown_ptr;		//0x0008
    int _unknown;			//0x000C
    Key key;				//0x0010
    Value value;			//0x0014
};

template <typename Key, typename Value>
struct Head_t
{
    Node_t<Key, Value>* memory;		//0x0000
    int allocation_count;			//0x0004
    int grow_size;					//0x0008
    int start_element;				//0x000C
    int next_available;				//0x0010
    int _unknown;					//0x0014
    int last_element;				//0x0018
}; //Size=0x001C

struct String_t
{
    char* buffer;	//0x0000
    int capacity;	//0x0004
    int grow_size;	//0x0008
    int length;		//0x000C
}; //Size=0x0010

struct CPaintKit
{
    int id;
    String_t name;
    String_t description;
    String_t item_name;
};

struct CStickerKit
{
    int id;

    int item_rarity;

    String_t name;
    String_t description;
    String_t item_name;
};

auto game_data::initialize_kits() -> void
{
    const auto V_UCS2ToUTF8 = static_cast<int(*)(const wchar_t* ucs2, char* utf8, int len)>(platform::get_export("vstdlib.dll", "V_UCS2ToUTF8"));
    const uintptr_t itemSchema = memory.itemSystem() + 4;

    // Dump paint kits
    {
        const auto map_head = reinterpret_cast<Head_t<int, CPaintKit*>*>(itemSchema + 0x28C);

        for (int i = 0; i <= map_head->last_element; i++) {
            const auto paint_kit = map_head->memory[i].value;

            if (paint_kit->id == 9001)
                continue;

            const auto wide_name = interfaces.localize->find(paint_kit->item_name.buffer + 1);
            char name[256];
            V_UCS2ToUTF8(wide_name, name, sizeof(name));

            if (paint_kit->id < 10000)
                game_data::skin_kits.emplace_back(paint_kit->id, name);
            else
                game_data::glove_kits.emplace_back(paint_kit->id, name);
        }

        std::sort(game_data::skin_kits.begin(), game_data::skin_kits.end());
        std::sort(game_data::glove_kits.begin(), game_data::glove_kits.end());
    }

    // Dump sticker kits
    {
        const auto map_head = reinterpret_cast<Head_t<int, CStickerKit*>*>(itemSchema + 0x2B0);

        game_data::sticker_kits.emplace_back(0, "None");

        for (int i = 0; i <= map_head->last_element; i++) {
            const auto sticker_kit = map_head->memory[i].value;

            char sticker_name_if_valve_fucked_up_their_translations[64];

            auto sticker_name_ptr = sticker_kit->item_name.buffer + 1;

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
}
