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

    // 

    struct GameItem {
        enum class Type {
            Sticker,
            Glove,
            Skin,
            Music
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

    struct MusicData {
        int musicID;
    };

    const MusicData& getMusicData(std::size_t index) noexcept;

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
}
