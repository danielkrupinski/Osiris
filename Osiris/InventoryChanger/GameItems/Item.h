#pragma once

#include <cstddef>
#include <string_view>

#include "../../SDK/ItemSchema.h"
#include "../../SDK/WeaponId.h"

namespace game_items
{

struct Item {
public:
    [[nodiscard]] static constexpr Item sticker(EconRarity rarity, std::size_t dataIndex, std::string_view iconPath) noexcept
    {
        return Item{ Type::Sticker, rarity, WeaponId::Sticker, dataIndex, iconPath };
    }

    [[nodiscard]] static constexpr Item skin(EconRarity rarity, WeaponId weaponID, std::size_t dataIndex, std::string_view iconPath) noexcept
    {
        return Item{ Type::Skin, rarity, weaponID, dataIndex, iconPath };
    }

    [[nodiscard]] static constexpr Item gloves(EconRarity rarity, WeaponId weaponID, std::size_t dataIndex, std::string_view iconPath) noexcept
    {
        return Item{ Type::Gloves, rarity, weaponID, dataIndex, iconPath };
    }

    [[nodiscard]] static constexpr Item musicKit(EconRarity rarity, std::size_t dataIndex, std::string_view iconPath) noexcept
    {
        return Item{ Type::Music, rarity, WeaponId::MusicKit, dataIndex, iconPath };
    }

    [[nodiscard]] static constexpr Item collectible(EconRarity rarity, WeaponId weaponID, std::size_t dataIndex, std::string_view iconPath) noexcept
    {
        return Item{ Type::Collectible, rarity, weaponID, dataIndex, iconPath };
    }

    [[nodiscard]] static constexpr Item nameTag(EconRarity rarity, WeaponId weaponID, std::size_t dataIndex, std::string_view iconPath) noexcept
    {
        return Item{ Type::NameTag, rarity, weaponID, dataIndex, iconPath };
    }

    [[nodiscard]] static constexpr Item patch(EconRarity rarity, std::size_t dataIndex, std::string_view iconPath) noexcept
    {
        return Item{ Type::Patch, rarity, WeaponId::Patch, dataIndex, iconPath };
    }

    [[nodiscard]] static constexpr Item graffiti(EconRarity rarity, std::size_t dataIndex, std::string_view iconPath) noexcept
    {
        return Item{ Type::Graffiti, rarity, WeaponId::Graffiti, dataIndex, iconPath };
    }

    [[nodiscard]] static constexpr Item sealedGraffiti(EconRarity rarity, std::size_t dataIndex, std::string_view iconPath) noexcept
    {
        return Item{ Type::SealedGraffiti, rarity, WeaponId::SealedGraffiti, dataIndex, iconPath };
    }

    [[nodiscard]] static constexpr Item agent(EconRarity rarity, WeaponId weaponID, std::size_t dataIndex, std::string_view iconPath) noexcept
    {
        return Item{ Type::Agent, rarity, weaponID, dataIndex, iconPath };
    }

    [[nodiscard]] static constexpr Item crate(EconRarity rarity, WeaponId weaponID, std::size_t dataIndex, std::string_view iconPath) noexcept
    {
        return Item{ Type::Case, rarity, weaponID, dataIndex, iconPath };
    }

    [[nodiscard]] static constexpr Item caseKey(EconRarity rarity, WeaponId weaponID, std::size_t dataIndex, std::string_view iconPath) noexcept
    {
        return Item{ Type::CaseKey, rarity, weaponID, dataIndex, iconPath };
    }

    [[nodiscard]] static constexpr Item operationPass(EconRarity rarity, WeaponId weaponID, std::size_t dataIndex, std::string_view iconPath) noexcept
    {
        return Item{ Type::OperationPass, rarity, weaponID, dataIndex, iconPath };
    }

    [[nodiscard]] static constexpr Item statTrakSwapTool(EconRarity rarity, WeaponId weaponID, std::size_t dataIndex, std::string_view iconPath) noexcept
    {
        return Item{ Type::StatTrakSwapTool, rarity, weaponID, dataIndex, iconPath };
    }

    [[nodiscard]] static constexpr Item viewerPass(EconRarity rarity, WeaponId weaponID, std::size_t dataIndex, std::string_view iconPath) noexcept
    {
        return Item{ Type::ViewerPass, rarity, weaponID, dataIndex, iconPath };
    }

    [[nodiscard]] static constexpr Item serviceMedal(EconRarity rarity, WeaponId weaponID, std::size_t dataIndex, std::string_view iconPath) noexcept
    {
        return Item{ Type::ServiceMedal, rarity, weaponID, dataIndex, iconPath };
    }

    [[nodiscard]] static constexpr Item souvenirToken(EconRarity rarity, WeaponId weaponID, std::size_t dataIndex, std::string_view iconPath) noexcept
    {
        return Item{ Type::SouvenirToken, rarity, weaponID, dataIndex, iconPath };
    }

    [[nodiscard]] static constexpr Item tournamentCoin(EconRarity rarity, WeaponId weaponID, std::size_t dataIndex, std::string_view iconPath) noexcept
    {
        return Item{ Type::TournamentCoin, rarity, weaponID, dataIndex, iconPath };
    }

    constexpr bool isSticker() const noexcept { return type == Type::Sticker; }
    constexpr bool isSkin() const noexcept { return type == Type::Skin; }
    constexpr bool isGloves() const noexcept { return type == Type::Gloves; }
    constexpr bool isMusic() const noexcept { return type == Type::Music; }
    constexpr bool isCollectible() const noexcept { return type == Type::Collectible; }
    constexpr bool isNameTag() const noexcept { return type == Type::NameTag; }
    constexpr bool isPatch() const noexcept { return type == Type::Patch; }
    constexpr bool isGraffiti() const noexcept { return type == Type::Graffiti; }
    constexpr bool isSealedGraffiti() const noexcept { return type == Type::SealedGraffiti; }
    constexpr bool isAgent() const noexcept { return type == Type::Agent; }
    constexpr bool isCase() const noexcept { return type == Type::Case; }
    constexpr bool isCaseKey() const noexcept { return type == Type::CaseKey; }
    constexpr bool isOperationPass() const noexcept { return type == Type::OperationPass; }
    constexpr bool isStatTrakSwapTool() const noexcept { return type == Type::StatTrakSwapTool; }
    constexpr bool isViewerPass() const noexcept { return type == Type::ViewerPass; }
    constexpr bool isServiceMedal() const noexcept { return type == Type::ServiceMedal; }
    constexpr bool isSouvenirToken() const noexcept { return type == Type::SouvenirToken; }
    constexpr bool isTournamentCoin() const noexcept { return type == Type::TournamentCoin; }

private:
    enum class Type : std::uint8_t {
        Gloves,
        Skin,
        Patch,

        Sticker,
        Music,
        Graffiti,
        SealedGraffiti,
        Collectible,
        NameTag,
        Agent,
        Case,
        CaseKey,
        OperationPass,
        StatTrakSwapTool,
        ViewerPass,
        ServiceMedal,
        SouvenirToken,
        TournamentCoin
    };

    constexpr Item(Type type, EconRarity rarity, WeaponId weaponID, std::size_t dataIndex, std::string_view iconPath) noexcept
        : type{ type }, rarity{ rarity }, weaponID{ weaponID }, dataIndex{ dataIndex }, iconPath{ iconPath } {}

    Type type;
    EconRarity rarity;
    WeaponId weaponID;
    std::size_t dataIndex;
    std::string_view iconPath;

public:
    [[nodiscard]] constexpr EconRarity getRarity() const noexcept { return rarity; }
    [[nodiscard]] constexpr WeaponId getWeaponID() const noexcept { return weaponID; }
    [[nodiscard]] constexpr std::size_t getDataIndex() const noexcept { return dataIndex; }
    [[nodiscard]] constexpr std::string_view getIconPath() const noexcept { return iconPath; }
};

}
