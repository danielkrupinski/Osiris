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
#include "item_definitions.hpp"
#include "../Hacks/SkinChanger.h"
#include <limits>
#include <unordered_map>
#include <array>
#include <algorithm>

#undef min

struct sticker_setting
{
	void update()
	{
        kit = SkinChanger::stickerKits[kit_vector_index].id;
	}

	int kit = 0;
	int kit_vector_index = 0;
	float wear = std::numeric_limits<float>::min();
	float scale = 1.f;
	float rotation = 0.f;
};

struct item_setting
{
	void update()
	{
        itemId = game_data::weapon_names[itemIdIndex].definition_index;
        quality = game_data::quality_names[entity_quality_vector_index].index;

		const std::vector<SkinChanger::PaintKit>* kit_names;
		const game_data::weapon_name* defindex_names;

		if (itemId == GLOVE_T_SIDE)
		{
			kit_names = &SkinChanger::gloveKits;
			defindex_names = game_data::glove_names;
		}
		else
		{
			kit_names = &SkinChanger::skinKits;
			defindex_names = game_data::knife_names;
		}

        paintKit = (*kit_names)[paint_kit_vector_index].id;
        definition_override_index = defindex_names[definition_override_vector_index].definition_index;

		for(auto& sticker : stickers)
			sticker.update();
	}

	bool enabled = false;
	int itemIdIndex = 0;
	int itemId = 1;
	int entity_quality_vector_index = 0;
	int quality = 0;
	int paint_kit_vector_index = 0;
	int paintKit = 0;
	int definition_override_vector_index = 0;
	int definition_override_index = 0;
	int seed = 0;
	int stat_trak = 0;
	float wear = std::numeric_limits<float>::min();
	char custom_name[32] = "";
	std::array<sticker_setting, 5> stickers;
};

item_setting* get_by_definition_index(int definition_index);
