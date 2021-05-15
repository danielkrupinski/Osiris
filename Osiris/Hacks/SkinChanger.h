#pragma once

#include <algorithm>
#include <array>
#include <iterator>
#include <limits>
#include <memory>
#include <string>
#include <vector>

#include "../imgui/imgui.h"

#include "../SDK/WeaponId.h"
#include "../JsonForward.h"

enum class FrameStage;
class Entity;
class GameEvent;

namespace InventoryChanger
{
    // Config
    json toJson() noexcept;
    void fromJson(const json& j) noexcept;
    void resetConfig() noexcept;
}

namespace SkinChanger
{
    void run(FrameStage) noexcept;
    void scheduleHudUpdate() noexcept;
    void overrideHudIcon(GameEvent& event) noexcept;
    void updateStatTrak(GameEvent& event) noexcept;

    // GUI
    void menuBarItem() noexcept;
    void tabItem() noexcept;
    void drawGUI(bool contentOnly) noexcept;

    enum class ItemType : short {
        Skin,
        Sticker
    };

    struct [[deprecated]] PaintKit {
        PaintKit(int id, std::wstring&& name, std::string&& iconPath, WeaponId weaponId, ItemType type, int rarity) noexcept;
        PaintKit(int id, std::wstring&& name, WeaponId weaponId, ItemType type, int rarity) noexcept;

        int id;
        WeaponId weaponId;
        ItemType type;
        int rarity;
        std::string name;
        std::wstring nameUpperCase;
        std::string iconPath;

        auto operator<(const PaintKit& other) const noexcept
        {
            return nameUpperCase < other.nameUpperCase;
        }
    };

    // 

    struct GameItem {
        enum class Type {
            Sticker,
            Glove,
            Skin
        };

        GameItem(Type type, int rarity, std::size_t dataIndex, std::wstring&& name, std::string&& iconPath) noexcept;

        Type type;
        int rarity;
        std::size_t dataIndex;

        std::string name;
        std::wstring nameUpperCase;
        std::string iconPath;
    };

    const std::vector<GameItem>& getGameItems() noexcept;

    struct StickerData {
        int stickerID;
    };

    const StickerData& getStickerData(std::size_t index) noexcept;

    struct GloveData {
        int paintKit;
        WeaponId weaponId;
    };

    const GloveData& getGloveData(std::size_t index) noexcept;

    struct SkinData {
        int paintKit;
        WeaponId weaponId;
    };

    const SkinData& getSkinData(std::size_t index) noexcept;

    void setStickerApplySlot(int slot) noexcept;
    void setToolToUse(std::uint64_t itemID) noexcept;
    void setItemToApplyTool(std::uint64_t itemID) noexcept;

    //

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

    const std::vector<Quality>& getQualities() noexcept;
    const std::vector<Item>& getGloveTypes() noexcept;
    const std::vector<Item>& getKnifeTypes() noexcept;

    ImTextureID getItemIconTexture(const std::string& iconpath) noexcept;
    void clearItemIconTextures() noexcept;
    void clearUnusedItemIconTextures() noexcept;

    void fixKnifeAnimation(Entity* viewModelWeapon, long& sequence) noexcept;

    struct [[deprecated]] weapon_name {
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
