#pragma once

#include <cstddef>
#include <string_view>

enum class WeaponId : short;
enum class EconRarity : std::uint8_t;

namespace game_items
{

struct Item {
public:
    [[nodiscard]] constexpr EconRarity getRarity() const noexcept { return rarity; }
    [[nodiscard]] constexpr WeaponId getWeaponID() const noexcept { return weaponID; }
    [[nodiscard]] constexpr std::size_t getDataIndex() const noexcept { return dataIndex; }
    [[nodiscard]] constexpr std::string_view getIconPath() const noexcept { return iconPath; }

    [[nodiscard]] constexpr bool isSticker() const noexcept { return type == Type::Sticker; }
    [[nodiscard]] constexpr bool isSkin() const noexcept { return type == Type::Skin; }
    [[nodiscard]] constexpr bool isGloves() const noexcept { return type == Type::Gloves; }
    [[nodiscard]] constexpr bool isMusic() const noexcept { return type == Type::Music; }
    [[nodiscard]] constexpr bool isCollectible() const noexcept { return type == Type::Collectible; }
    [[nodiscard]] constexpr bool isNameTag() const noexcept { return type == Type::NameTag; }
    [[nodiscard]] constexpr bool isPatch() const noexcept { return type == Type::Patch; }
    [[nodiscard]] constexpr bool isGraffiti() const noexcept { return type == Type::Graffiti; }
    [[nodiscard]] constexpr bool isAgent() const noexcept { return type == Type::Agent; }
    [[nodiscard]] constexpr bool isCase() const noexcept { return type == Type::Case; }
    [[nodiscard]] constexpr bool isCaseKey() const noexcept { return type == Type::CaseKey; }
    [[nodiscard]] constexpr bool isOperationPass() const noexcept { return type == Type::OperationPass; }
    [[nodiscard]] constexpr bool isStatTrakSwapTool() const noexcept { return type == Type::StatTrakSwapTool; }
    [[nodiscard]] constexpr bool isViewerPass() const noexcept { return type == Type::ViewerPass; }
    [[nodiscard]] constexpr bool isServiceMedal() const noexcept { return type == Type::ServiceMedal; }
    [[nodiscard]] constexpr bool isSouvenirToken() const noexcept { return type == Type::SouvenirToken; }
    [[nodiscard]] constexpr bool isTournamentCoin() const noexcept { return type == Type::TournamentCoin; }

    enum class Type : std::uint8_t {
        Gloves,
        Skin,
        Patch,
        Sticker,
        Music,
        Graffiti,
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

    [[nodiscard]] constexpr Item(Type type, EconRarity rarity, WeaponId weaponID, std::size_t dataIndex, std::string_view iconPath) noexcept
        : type{ type }, rarity{ rarity }, weaponID{ weaponID }, dataIndex{ dataIndex }, iconPath{ iconPath } {}

private:
    Type type;
    EconRarity rarity;
    WeaponId weaponID;
    std::size_t dataIndex;
    std::string_view iconPath;
};

}
