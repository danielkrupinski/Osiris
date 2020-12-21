#pragma once

#include <array>
#include <string>
#include <vector>

#include "../nSkinz/item_definitions.hpp"

enum class FrameStage;
class GameEvent;

namespace SkinChanger
{
    void run(FrameStage) noexcept;
    void scheduleHudUpdate() noexcept;
    void overrideHudIcon(GameEvent& event) noexcept;
    void updateStatTrak(GameEvent& event) noexcept;

    struct PaintKit {
        [[deprecated]] PaintKit(int id, const std::string& name, const std::wstring& nameUpperCase) noexcept : id(id), name(name), nameUpperCase(nameUpperCase) { }
        PaintKit(int id, const std::string& name) noexcept;
        PaintKit(int id, std::string&& name) noexcept;

        int id;
        std::string name;
        std::wstring nameUpperCase;

        auto operator<(const PaintKit& other) const noexcept
        {
            return nameUpperCase < other.nameUpperCase;
        }
    };

    struct Quality {
        Quality(int index, const char* name) : index{ index }, name{ name } {}
        int index = 0;
        std::string name;
    };

    const std::vector<PaintKit>& getSkinKits() noexcept;
    const std::vector<PaintKit>& getGloveKits() noexcept;
    const std::vector<PaintKit>& getStickerKits() noexcept;
    const std::vector<Quality>& getQualities() noexcept;
}


// FROM nSkinz

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

struct sticker_setting
{
    void update()
    {
        kit = SkinChanger::getStickerKits()[kit_vector_index].id;
    }

    void onLoad()
    {
        const auto& kits = SkinChanger::getStickerKits();
        const auto it = std::find_if(kits.begin(), kits.end(), [this](const auto& k) { return k.id == kit; });
        kit_vector_index = it != SkinChanger::getStickerKits().end() ? std::distance(kits.begin(), it) : 0;
        kit = SkinChanger::getStickerKits()[kit_vector_index].id;
    }

    auto operator==(const sticker_setting& o) const
    {
        return kit == o.kit
            && kit_vector_index == o.kit_vector_index
            && wear == o.wear
            && scale == o.scale
            && rotation == o.rotation;
    }

    int kit = 0;
    int kit_vector_index = 0;
    float wear = (std::numeric_limits<float>::min)();
    float scale = 1.f;
    float rotation = 0.f;
};

struct item_setting {
    void update()
    {
        itemId = game_data::weapon_names[itemIdIndex].definition_index;
        quality = SkinChanger::getQualities()[entity_quality_vector_index].index;

        if (itemId == GLOVE_T_SIDE) {
            paintKit = SkinChanger::getGloveKits()[paint_kit_vector_index].id;
            definition_override_index = game_data::glove_names[definition_override_vector_index].definition_index;
        } else {
            paintKit = SkinChanger::getSkinKits()[paint_kit_vector_index].id;
            definition_override_index = game_data::knife_names[definition_override_vector_index].definition_index;
        }

        for (auto& sticker : stickers)
            sticker.update();
    }

    void onLoad()
    {
        {
            const auto it = std::find_if(std::begin(game_data::weapon_names), std::end(game_data::weapon_names), [this](const auto& k) { return k.definition_index == itemId; });
            itemIdIndex = it != std::end(game_data::weapon_names) ? std::distance(std::begin(game_data::weapon_names), it) : 0;
        }

        {
            const auto& qualities = SkinChanger::getQualities();
            const auto it = std::find_if(qualities.begin(), qualities.end(), [this](const auto& k) { return k.index == quality; });
            entity_quality_vector_index = it != qualities.end() ? std::distance(qualities.begin(), it) : 0;
        }

        if (itemId == GLOVE_T_SIDE) {
            {
                const auto it = std::find_if(SkinChanger::getGloveKits().begin(), SkinChanger::getGloveKits().end(), [this](const auto& k) { return k.id == paintKit; });
                paint_kit_vector_index = it != SkinChanger::getGloveKits().end() ? std::distance(SkinChanger::getGloveKits().begin(), it) : 0;
            }

            {
                const auto it = std::find_if(game_data::glove_names.begin(), game_data::glove_names.end(), [this](const auto& k) { return k.definition_index == definition_override_index; });
                definition_override_vector_index = it != game_data::glove_names.end() ? std::distance(game_data::glove_names.begin(), it) : 0;
            }
        } else {
            {
                const auto it = std::find_if(SkinChanger::getSkinKits().begin(), SkinChanger::getSkinKits().end(), [this](const auto& k) { return k.id == paintKit; });
                paint_kit_vector_index = it != SkinChanger::getSkinKits().end() ? std::distance(SkinChanger::getSkinKits().begin(), it) : 0;
            }

            {
                const auto it = std::find_if(game_data::knife_names.begin(), game_data::knife_names.end(), [this](const auto& k) { return k.definition_index == definition_override_index; });
                definition_override_vector_index = it != game_data::knife_names.end() ? std::distance(game_data::knife_names.begin(), it) : 0;
            }
        }

        for (auto& sticker : stickers)
            sticker.onLoad();
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
    int stat_trak = -1;
    float wear = (std::numeric_limits<float>::min)();
    char custom_name[32] = "";
    std::array<sticker_setting, 5> stickers;
};

item_setting* get_by_definition_index(int definition_index);
