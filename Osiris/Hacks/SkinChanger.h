#pragma once

#include <algorithm>
#include <array>
#include <string>
#include <vector>

#include "../imgui/imgui.h"

#include "../SDK/WeaponId.h"

enum class FrameStage;
class Entity;
class GameEvent;

namespace SkinChanger
{
    void run(FrameStage) noexcept;
    void scheduleHudUpdate() noexcept;
    void overrideHudIcon(GameEvent& event) noexcept;
    void updateStatTrak(GameEvent& event) noexcept;

    struct PaintKit {
        PaintKit(int id, const std::string& name, int rarity = 0) noexcept;
        PaintKit(int id, std::string&& name, int rarity = 0) noexcept;
        PaintKit(int id, std::wstring&& name, std::string&& iconPath, int rarity = 0) noexcept;
        PaintKit(int id, std::wstring&& name, int rarity = 0) noexcept;

        int id;
        int rarity;
        std::string name;
        std::wstring nameUpperCase;
        std::string iconPath;

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

    struct Item {
        Item(WeaponId id, const char* name) : id(id), name(name) {}

        WeaponId id;
        std::string name;
    };

    const std::vector<PaintKit>& getSkinKits() noexcept;
    const std::vector<PaintKit>& getGloveKits() noexcept;
    const std::vector<PaintKit>& getStickerKits() noexcept;
    const std::vector<Quality>& getQualities() noexcept;
    const std::vector<Item>& getGloveTypes() noexcept;
    const std::vector<Item>& getKnifeTypes() noexcept;

    ImTextureID getItemIconTexture(const std::string& iconpath) noexcept;
    void clearItemIconTextures() noexcept;

    void fixKnifeAnimation(Entity* viewModelWeapon, long& sequence) noexcept;

    struct weapon_name {
        constexpr weapon_name(WeaponId definition_index, const char* name) : definition_index(definition_index), name(name) {}

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
        const auto it = std::ranges::find(kits, kit, &SkinChanger::PaintKit::id);
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
        itemId = SkinChanger::weapon_names[itemIdIndex].definition_index;
        quality = SkinChanger::getQualities()[entity_quality_vector_index].index;

        if (itemId == WeaponId::GloveT) {
            paintKit = SkinChanger::getGloveKits()[paint_kit_vector_index].id;
            definition_override_index = (int)SkinChanger::getGloveTypes()[definition_override_vector_index].id;
        } else {
            paintKit = SkinChanger::getSkinKits()[paint_kit_vector_index].id;
            definition_override_index = (int)SkinChanger::getKnifeTypes()[definition_override_vector_index].id;
        }

        for (auto& sticker : stickers)
            sticker.update();
    }

    void onLoad()
    {
        {
            const auto it = std::ranges::find(SkinChanger::weapon_names, itemId, &SkinChanger::weapon_name::definition_index);
            itemIdIndex = it != SkinChanger::weapon_names.end() ? std::distance(SkinChanger::weapon_names.begin(), it) : 0;
        }

        {
            const auto& qualities = SkinChanger::getQualities();
            const auto it = std::ranges::find(qualities, quality, &SkinChanger::Quality::index);
            entity_quality_vector_index = it != qualities.end() ? std::distance(qualities.begin(), it) : 0;
        }

        if (itemId == WeaponId::GloveT) {
            {
                const auto it = std::ranges::find(SkinChanger::getGloveKits(), paintKit, &SkinChanger::PaintKit::id);
                paint_kit_vector_index = it != SkinChanger::getGloveKits().end() ? std::distance(SkinChanger::getGloveKits().begin(), it) : 0;
            }

            {
                const auto it = std::ranges::find(SkinChanger::getGloveTypes(), static_cast<WeaponId>(definition_override_index), &SkinChanger::Item::id);
                definition_override_vector_index = it != SkinChanger::getGloveTypes().end() ? std::distance(SkinChanger::getGloveTypes().begin(), it) : 0;
            }
        } else {
            {
                const auto it = std::ranges::find(SkinChanger::getSkinKits(), paintKit, &SkinChanger::PaintKit::id);
                paint_kit_vector_index = it != SkinChanger::getSkinKits().end() ? std::distance(SkinChanger::getSkinKits().begin(), it) : 0;
            }

            {
                const auto it = std::ranges::find(SkinChanger::getKnifeTypes(), static_cast<WeaponId>(definition_override_index), &SkinChanger::Item::id);
                definition_override_vector_index = it != SkinChanger::getKnifeTypes().end() ? std::distance(SkinChanger::getKnifeTypes().begin(), it) : 0;
            }
        }

        for (auto& sticker : stickers)
            sticker.onLoad();
    }

    bool enabled = false;
    int itemIdIndex = 0;
    WeaponId itemId{};
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

item_setting* get_by_definition_index(WeaponId weaponId);
