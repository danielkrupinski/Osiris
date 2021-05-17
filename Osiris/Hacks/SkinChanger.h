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

    void clearInventory() noexcept;
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

    // 

    struct GameItem {
        enum class Type {
            Sticker,
            Glove,
            Skin,
            Music
        };

        GameItem(Type type, int rarity, std::size_t dataIndex, std::wstring&& name, std::string&& iconPath) noexcept;

        bool isSticker() const noexcept { return type == Type::Sticker; }
        bool isSkin() const noexcept { return type == Type::Skin; }
        bool isGlove() const noexcept { return type == Type::Glove; }
        bool isMusic() const noexcept { return type == Type::Music; }

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

    ImTextureID getItemIconTexture(const std::string& iconpath) noexcept;
    void clearItemIconTextures() noexcept;
    void clearUnusedItemIconTextures() noexcept;

    void fixKnifeAnimation(Entity* viewModelWeapon, long& sequence) noexcept;
}
