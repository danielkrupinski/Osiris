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
#include "config.hpp"
#include "SDK.hpp"

#include <fstream>
#include <nlohmann/json.hpp>

config g_config;

using json = nlohmann::json;

#define TO_JSON_HELPER(var_name) {#var_name, o.var_name}
#define FROM_JSON_HELPER(var_name) {const auto it = j.find(#var_name); if(it != std::end(j)) o.var_name = it->get<decltype(o.var_name)>();}
#define FROM_JSON_HELPER_STR(var_name) {const auto it = j.find(#var_name); if(it != std::end(j)) strcpy_s(o.var_name, it->get<std::string>().c_str());}

auto to_json(json& j, const sticker_setting& o) -> void
{
	j = json
	{
		TO_JSON_HELPER(kit),
		TO_JSON_HELPER(wear),
		TO_JSON_HELPER(scale),
		TO_JSON_HELPER(rotation),
	};
}

auto from_json(const json& j, sticker_setting& o) -> void
{
	FROM_JSON_HELPER(kit);
	FROM_JSON_HELPER(wear);
	FROM_JSON_HELPER(scale);
	FROM_JSON_HELPER(rotation);
	o.update<sync_type::VALUE_TO_KEY>();
}

auto to_json(json& j, const item_setting& o) -> void
{
	j = json
	{
		TO_JSON_HELPER(name),
		TO_JSON_HELPER(enabled),
		TO_JSON_HELPER(definition_index),
		TO_JSON_HELPER(entity_quality_index),
		TO_JSON_HELPER(paint_kit_index),
		TO_JSON_HELPER(definition_override_index),
		TO_JSON_HELPER(seed),
		TO_JSON_HELPER(stat_trak),
		TO_JSON_HELPER(wear),
		TO_JSON_HELPER(custom_name),
		TO_JSON_HELPER(stickers),
	};
}

auto from_json(const json& j, item_setting& o) -> void
{
	FROM_JSON_HELPER_STR(name);
	FROM_JSON_HELPER(enabled);
	FROM_JSON_HELPER(definition_index);
	FROM_JSON_HELPER(entity_quality_index);
	FROM_JSON_HELPER(paint_kit_index);
	FROM_JSON_HELPER(definition_override_index);
	FROM_JSON_HELPER(seed);
	FROM_JSON_HELPER(stat_trak);
	FROM_JSON_HELPER(wear);
	FROM_JSON_HELPER_STR(custom_name);
	const auto sticker_it = j.find("stickers");
	if(sticker_it != std::end(j))
	{
		const auto stickers = sticker_it->get<std::vector<sticker_setting>>();

		const auto size = std::min(stickers.size(), o.stickers.size());

		for(auto i = 0u; i < size; ++i)
			o.stickers.at(i) = stickers.at(i);
	}

	o.update<sync_type::VALUE_TO_KEY>();
}

auto config::save() -> void
{
	auto of = std::ofstream("nSkinz.json");
	if(of.good())
		of << json(m_items);
}

auto config::load() -> void
{
	try
	{
		auto ifile = std::ifstream("nSkinz.json");
		if(ifile.good())
		{
			m_items = json::parse(ifile).get<std::vector<item_setting>>();
			(*g_client_state)->ForceFullUpdate();
		}
	}
	catch(const std::exception&)
	{
		// Config file doesn't exists or is malformed, just ignore it
		// This will probably crash if you use a manual mapper that doesnt do proper exception handling
	}
}

auto config::get_by_definition_index(const int definition_index) -> item_setting*
{
	auto it = std::find_if(m_items.begin(), m_items.end(), [definition_index](const item_setting& e)
	{
		return e.enabled && e.definition_index == definition_index;
	});

	return it == m_items.end() ? nullptr : &*it;
}
