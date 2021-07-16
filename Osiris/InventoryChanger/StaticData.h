#pragma once

#include <vector>

#include "../SDK/WeaponId.h"

namespace StaticData
{
    constexpr auto InvalidItemIdx = static_cast<std::size_t>(-1);

    enum class Type : std::uint8_t {
        // has paint kit, must match GameItem::hasPaintKit() below
        Sticker,
        Glove,
        Skin,
        Music,
        Patch,
        Graffiti,
        SealedGraffiti,

        // has other data
        Collectible,
        NameTag,
        Agent,
        Case,
        CaseKey,
        OperationPass,
        StatTrakSwapTool,
        ViewerPass
    };

    struct GameItem {
        GameItem(Type type, int rarity, WeaponId weaponID, std::size_t dataIndex, std::string&& iconPath) noexcept;

        bool isSticker() const noexcept { return type == Type::Sticker; }
        bool isSkin() const noexcept { return type == Type::Skin; }
        bool isGlove() const noexcept { return type == Type::Glove; }
        bool isMusic() const noexcept { return type == Type::Music; }
        bool isCollectible() const noexcept { return type == Type::Collectible; }
        bool isNameTag() const noexcept { return type == Type::NameTag; }
        bool isPatch() const noexcept { return type == Type::Patch; }
        bool isGraffiti() const noexcept { return type == Type::Graffiti; }
        bool isSealedGraffiti() const noexcept { return type == Type::SealedGraffiti; }
        bool isAgent() const noexcept { return type == Type::Agent; }
        bool isCase() const noexcept { return type == Type::Case; }
        bool isCaseKey() const noexcept { return type == Type::CaseKey; }
        bool isOperationPass() const noexcept { return type == Type::OperationPass; }
        bool isStatTrakSwapTool() const noexcept { return type == Type::StatTrakSwapTool; }
        bool isViewerPass() const noexcept { return type == Type::ViewerPass; }

        bool hasPaintKit() const noexcept { return type >= Type::Sticker && type <= Type::SealedGraffiti; }

        Type type;
        std::uint8_t rarity;
        WeaponId weaponID;
        std::size_t dataIndex;

        std::string iconPath;
    };

    struct Collectible {
        explicit Collectible(bool isOriginal) : isOriginal{ isOriginal } {}

        bool isOriginal;
    };

    struct PaintKit {
        PaintKit(int id, std::wstring&& name, float wearRemapMin = 0.0f, float wearRemapMax = 1.0f) noexcept;

        int id;
        float wearRemapMin;
        float wearRemapMax;
        std::string name;
        std::wstring nameUpperCase;
    };

    struct Case {
        bool willProduceStatTrak = false;
        bool isSouvenirPackage = false;
        std::size_t lootBeginIdx = 0;
        std::size_t lootEndIdx = 0;

        bool hasLoot() const noexcept { return lootEndIdx > lootBeginIdx; }
    };

    const std::vector<GameItem>& gameItems() noexcept;
    const std::vector<Collectible>& collectibles() noexcept;
    const std::vector<Case>& cases() noexcept;
    const std::vector<std::size_t>& caseLoot() noexcept;
    const std::vector<PaintKit>& paintKits() noexcept;
    const std::wstring& getWeaponNameUpper(WeaponId weaponID) noexcept;
    const std::string& getWeaponName(WeaponId weaponID) noexcept;

    std::size_t getItemIndex(WeaponId weaponID, int paintKit) noexcept;
}
