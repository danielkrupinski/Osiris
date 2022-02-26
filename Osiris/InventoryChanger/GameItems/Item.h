#pragma once

#include <cstddef>
#include <string_view>

#include "../../SDK/ItemSchema.h"
#include "../../SDK/WeaponId.h"

namespace game_items
{

struct GameItem {
public:
    [[nodiscard]] static constexpr GameItem sticker(EconRarity rarity, std::size_t dataIndex, std::string_view iconPath) noexcept
    {
        return GameItem{ Type::Sticker, rarity, WeaponId::Sticker, dataIndex, iconPath };
    }

    [[nodiscard]] static constexpr GameItem skin(EconRarity rarity, WeaponId weaponID, std::size_t dataIndex, std::string_view iconPath) noexcept
    {
        return GameItem{ Type::Skin, rarity, weaponID, dataIndex, iconPath };
    }

    [[nodiscard]] static constexpr GameItem gloves(EconRarity rarity, WeaponId weaponID, std::size_t dataIndex, std::string_view iconPath) noexcept
    {
        return GameItem{ Type::Gloves, rarity, weaponID, dataIndex, iconPath };
    }

    [[nodiscard]] static constexpr GameItem musicKit(EconRarity rarity, std::size_t dataIndex, std::string_view iconPath) noexcept
    {
        return GameItem{ Type::Music, rarity, WeaponId::MusicKit, dataIndex, iconPath };
    }

    [[nodiscard]] static constexpr GameItem collectible(EconRarity rarity, WeaponId weaponID, std::size_t dataIndex, std::string_view iconPath) noexcept
    {
        return GameItem{ Type::Collectible, rarity, weaponID, dataIndex, iconPath };
    }

    [[nodiscard]] static constexpr GameItem nameTag(EconRarity rarity, WeaponId weaponID, std::size_t dataIndex, std::string_view iconPath) noexcept
    {
        return GameItem{ Type::NameTag, rarity, weaponID, dataIndex, iconPath };
    }

    [[nodiscard]] static constexpr GameItem patch(EconRarity rarity, std::size_t dataIndex, std::string_view iconPath) noexcept
    {
        return GameItem{ Type::Patch, rarity, WeaponId::Patch, dataIndex, iconPath };
    }

    [[nodiscard]] static constexpr GameItem graffiti(EconRarity rarity, std::size_t dataIndex, std::string_view iconPath) noexcept
    {
        return GameItem{ Type::Graffiti, rarity, WeaponId::Graffiti, dataIndex, iconPath };
    }

    [[nodiscard]] static constexpr GameItem sealedGraffiti(EconRarity rarity, std::size_t dataIndex, std::string_view iconPath) noexcept
    {
        return GameItem{ Type::SealedGraffiti, rarity, WeaponId::SealedGraffiti, dataIndex, iconPath };
    }

    [[nodiscard]] static constexpr GameItem agent(EconRarity rarity, WeaponId weaponID, std::size_t dataIndex, std::string_view iconPath) noexcept
    {
        return GameItem{ Type::Agent, rarity, weaponID, dataIndex, iconPath };
    }

    [[nodiscard]] static constexpr GameItem crate(EconRarity rarity, WeaponId weaponID, std::size_t dataIndex, std::string_view iconPath) noexcept
    {
        return GameItem{ Type::Case, rarity, weaponID, dataIndex, iconPath };
    }

    [[nodiscard]] static constexpr GameItem caseKey(EconRarity rarity, WeaponId weaponID, std::size_t dataIndex, std::string_view iconPath) noexcept
    {
        return GameItem{ Type::CaseKey, rarity, weaponID, dataIndex, iconPath };
    }

    [[nodiscard]] static constexpr GameItem operationPass(EconRarity rarity, WeaponId weaponID, std::size_t dataIndex, std::string_view iconPath) noexcept
    {
        return GameItem{ Type::OperationPass, rarity, weaponID, dataIndex, iconPath };
    }

    [[nodiscard]] static constexpr GameItem statTrakSwapTool(EconRarity rarity, WeaponId weaponID, std::size_t dataIndex, std::string_view iconPath) noexcept
    {
        return GameItem{ Type::StatTrakSwapTool, rarity, weaponID, dataIndex, iconPath };
    }

    [[nodiscard]] static constexpr GameItem viewerPass(EconRarity rarity, WeaponId weaponID, std::size_t dataIndex, std::string_view iconPath) noexcept
    {
        return GameItem{ Type::ViewerPass, rarity, weaponID, dataIndex, iconPath };
    }

    [[nodiscard]] static constexpr GameItem serviceMedal(EconRarity rarity, WeaponId weaponID, std::size_t dataIndex, std::string_view iconPath) noexcept
    {
        return GameItem{ Type::ServiceMedal, rarity, weaponID, dataIndex, iconPath };
    }

    [[nodiscard]] static constexpr GameItem souvenirToken(EconRarity rarity, WeaponId weaponID, std::size_t dataIndex, std::string_view iconPath) noexcept
    {
        return GameItem{ Type::SouvenirToken, rarity, weaponID, dataIndex, iconPath };
    }

    [[nodiscard]] static constexpr GameItem tournamentCoin(EconRarity rarity, WeaponId weaponID, std::size_t dataIndex, std::string_view iconPath) noexcept
    {
        return GameItem{ Type::TournamentCoin, rarity, weaponID, dataIndex, iconPath };
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

    constexpr GameItem(Type type, EconRarity rarity, WeaponId weaponID, std::size_t dataIndex, std::string_view iconPath) noexcept
        : type{ type }, rarity{ rarity }, weaponID{ weaponID }, dataIndex{ dataIndex }, iconPath{ iconPath } {}

    Type type;
public:
    EconRarity rarity;
    WeaponId weaponID;
    std::size_t dataIndex;

    std::string_view iconPath;
};

}
