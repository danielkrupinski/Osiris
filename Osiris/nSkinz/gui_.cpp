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
#include "../Config.h"
#include "config_.hpp"
#include "item_definitions.hpp"
#include "kit_parser.hpp"
#include "../Interfaces.h"
#include "../SDK/Cvar.h"
#include "../Hacks/SkinChanger.h"

#include "../imgui/imgui.h"
#include <functional>

namespace ImGui
{
	// ImGui ListBox lambda binder
	static bool ListBox(const char* label, int* current_item,  std::function<const char*(int)> lambda, int items_count, int height_in_items)
	{
		return ImGui::ListBox(label, current_item, [](void* data, int idx, const char** out_text)
		{
			*out_text = (*reinterpret_cast<std::function<const char*(int)>*>(data))(idx);
			return true;
		}, &lambda, items_count, height_in_items);
	}
}

void draw_gui()
{
	ImGui::SetNextWindowSize(ImVec2(700, 400));
	if (ImGui::Begin("nSkinz", nullptr,
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_AlwaysAutoResize |
		ImGuiWindowFlags_NoSavedSettings))
	{
		ImGui::Columns(2, nullptr, false);

        static auto itemIndex = 0;
        // Item to change skins for
        ImGui::Combo("Item", &itemIndex, [](void* data, int idx, const char** out_text)
            {
                *out_text = game_data::weapon_names[idx].name;
                return true;
            }, nullptr, sizeof(game_data::weapon_names) / sizeof(game_data::weapon_name), 5);


        auto& selected_entry = config.skinChanger.items[itemIndex];
        selected_entry.definition_vector_index = itemIndex;

		{
			ImGui::Checkbox("Enabled", &selected_entry.enabled);
			ImGui::InputInt("Seed", &selected_entry.seed);
			ImGui::InputInt("StatTrak", &selected_entry.stat_trak);
			ImGui::SliderFloat("Wear", &selected_entry.wear, FLT_MIN, 1.f, "%.10f", 5);

			// Paint kit
			if (itemIndex != 1) {
				ImGui::Combo("Paint Kit", &selected_entry.paint_kit_vector_index, [](void* data, int idx, const char** out_text)
				{
					*out_text = game_data::skin_kits[idx].name.c_str();
					return true;
				}, nullptr, game_data::skin_kits.size(), 10);
			} else {
				ImGui::Combo("Paint Kit", &selected_entry.paint_kit_vector_index, [](void* data, int idx, const char** out_text)
				{
					*out_text = game_data::glove_kits[idx].name.c_str();
					return true;
				}, nullptr, game_data::glove_kits.size(), 10);
			}

			// Quality
			ImGui::Combo("Quality", &selected_entry.entity_quality_vector_index, [](void* data, int idx, const char** out_text)
			{
				*out_text = game_data::quality_names[idx].name;
				return true;
			}, nullptr, sizeof(game_data::quality_names) / sizeof(game_data::quality_name), 5);

			// Yes we do it twice to decide knifes
			selected_entry.update();

			// Item defindex override
			if (itemIndex == 0) {
				ImGui::Combo("Knife", &selected_entry.definition_override_vector_index, [](void* data, int idx, const char** out_text)
				{
					*out_text = game_data::knife_names[idx].name;
					return true;
				}, nullptr, sizeof(game_data::knife_names) / sizeof(game_data::weapon_name), 5);
			} else if(itemIndex == 1) {
				ImGui::Combo("Glove", &selected_entry.definition_override_vector_index, [](void* data, int idx, const char** out_text)
				{
					*out_text = game_data::glove_names[idx].name;
					return true;
				}, nullptr, sizeof(game_data::glove_names) / sizeof(game_data::weapon_name), 5);
			} else {
				// We don't want to override weapons other than knives or gloves
				static auto unused_value = 0;
				selected_entry.definition_override_vector_index = 0;
				ImGui::Combo("Unavailable", &unused_value, "For knives or gloves\0");
			}

			selected_entry.update();

			// Custom Name tag
			ImGui::InputText("Name Tag", selected_entry.custom_name, 32);
		}

		ImGui::NextColumn();

		//ImGui::Columns(1, nullptr, false);

		//ImGui::Separator();

		{
			//ImGui::Columns(2, nullptr, false);

			ImGui::PushID("sticker");

			static auto selected_sticker_slot = 0;

			auto& selected_sticker = selected_entry.stickers[selected_sticker_slot];

			ImGui::PushItemWidth(-1);

			char element_name[64];

			ImGui::ListBox("", &selected_sticker_slot, [&selected_entry, &element_name](int idx)
			{
				auto kit_vector_index = selected_entry.stickers[idx].kit_vector_index;
				sprintf_s(element_name, "#%d (%s)", idx + 1, game_data::sticker_kits.at(kit_vector_index).name.c_str());
				return element_name;
			}, 5, 5);
			ImGui::PopItemWidth();

			//ImGui::NextColumn();

			ImGui::Combo("Sticker Kit", &selected_sticker.kit_vector_index, [](void* data, int idx, const char** out_text)
			{
				*out_text = game_data::sticker_kits.at(idx).name.c_str();
				return true;
			}, nullptr, game_data::sticker_kits.size(), 10);

			ImGui::SliderFloat("Wear", &selected_sticker.wear, FLT_MIN, 1.f, "%.10f", 5);

			ImGui::SliderFloat("Scale", &selected_sticker.scale, 0.1f, 5.f, "%.3f");

			ImGui::SliderFloat("Rotation", &selected_sticker.rotation, 0.f, 360.f);

			//ImGui::NextColumn();

			ImGui::PopID();
		}

		ImGui::Columns(1, nullptr, false);

		ImGui::Separator();

            if (ImGui::Button("Update", { 130.0f, 30.0f })) {
                interfaces.cvar->findVar("cl_fullupdate")->callBack();
                SkinChanger::scheduleHudUpdate();
            }

		ImGui::Text("nSkinz by namazso");

		ImGui::End();
	}

}
