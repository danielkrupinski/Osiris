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
#include "kit_parser.hpp"

#include <unordered_map>
#include <array>
#include <algorithm>

template<typename Container, typename T1, typename T2, typename TC>
class value_syncer
{
	using container_type = typename Container::value_type;

	const Container& container;
	T1& key;
	T2& value;
	const TC container_type::* member;

public:
	value_syncer(const Container& container, T1& key, T2& value, const TC container_type::* member)
		: container{container}
		, key{key}
		, value{value}
		, member{member}
	{}

	auto key_to_value() const -> void
	{
		key = std::clamp(key, T1(0), T1(container.size() - 1));
		value = container.at(key).*member;
	}

	auto value_to_key() const -> void
	{
		auto it = std::find_if(std::begin(container), std::end(container), [this](const container_type& x)
		{
			return value == x.*member;
		});

		// Originally I wanted this to work with maps too, but fuck that
		if(it != std::end(container))
			key = it - std::begin(container);
		else
			key = T1(0);
	}
};

enum class sync_type
{
	VALUE_TO_KEY,
	KEY_TO_VALUE
};

template<sync_type Type, typename Container, typename T1, typename T2, typename TC>
static auto do_sync(const Container& container, T1& key, T2& value, TC Container::value_type::* member) -> void
{
	auto syncer = value_syncer<Container, T1, T2, TC>{ container, key, value, member };
	if constexpr(Type == sync_type::VALUE_TO_KEY)
		syncer.value_to_key();
	else
		syncer.key_to_value();
}

struct sticker_setting
{
	template<sync_type Type>
	void update()
	{
		do_sync<Type>(game_data::sticker_kits, kit_vector_index, kit, &game_data::paint_kit::id);
	}

	int kit = 0;
	int kit_vector_index = 0;
	float wear = std::numeric_limits<float>::min();
	float scale = 1.f;
	float rotation = 0.f;
};

struct item_setting
{
	template<sync_type Type>
	void update()
	{
		do_sync<Type>(
			game_data::weapon_names,
			definition_vector_index,
			definition_index,
			&game_data::weapon_name::definition_index
		);

		do_sync<Type>(
			game_data::quality_names,
			entity_quality_vector_index,
			entity_quality_index,
			&game_data::quality_name::index
		);

		const std::vector<game_data::paint_kit>* kit_names;
		const std::vector<game_data::weapon_name>* defindex_names;

		if(definition_index == GLOVE_T_SIDE)
		{
			kit_names = &game_data::glove_kits;
			defindex_names = &game_data::glove_names;
		}
		else
		{
			kit_names = &game_data::skin_kits;
			defindex_names = &game_data::knife_names;
		}

		do_sync<Type>(
			*kit_names,
			paint_kit_vector_index,
			paint_kit_index,
			&game_data::paint_kit::id
		);

		do_sync<Type>(
			*defindex_names,
			definition_override_vector_index,
			definition_override_index,
			&game_data::weapon_name::definition_index
		);

		for(auto& sticker : stickers)
			sticker.update<Type>();
	}

	char name[32] = "Default";
	bool enabled = false;
	int definition_vector_index = 0;
	int definition_index = 1;
	int entity_quality_vector_index = 0;
	int entity_quality_index = 0;
	int paint_kit_vector_index = 0;
	int paint_kit_index = 0;
	int definition_override_vector_index = 0;
	int definition_override_index = 0;
	int seed = 0;
	int stat_trak = 0;
	float wear = std::numeric_limits<float>::min();
	char custom_name[32] = "";
	std::array<sticker_setting, 5> stickers;
};

class config
{
public:
	config()
	{
		// Ghetto fix for possible race conditions
		m_items.reserve(128);

		// Default config
		m_items.push_back(item_setting());
	}

	auto save() -> void;
	auto load() -> void;

	auto get_by_definition_index(int definition_index) -> item_setting*;

	auto get_items() -> std::vector<item_setting>&
	{
		return m_items;
	}

	auto get_icon_override_map() -> std::unordered_map<std::string_view, std::string_view>&
	{
		return m_icon_overrides;
	}

	auto get_icon_override(const std::string_view original) const -> const char*
	{
		return m_icon_overrides.count(original) ? m_icon_overrides.at(original).data() : nullptr;
	}

private:
	std::vector<item_setting> m_items;
	std::unordered_map<std::string_view, std::string_view> m_icon_overrides;
};

extern config g_config;