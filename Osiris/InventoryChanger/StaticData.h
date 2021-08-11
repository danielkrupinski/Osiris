#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

#include "../SDK/WeaponId.h"

enum TournamentTeam : std::uint8_t;

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
        ViewerPass,
        ServiceMedal
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
        bool isServiceMedal() const noexcept { return type == Type::ServiceMedal; }

        bool hasPaintKit() const noexcept { return type >= Type::Sticker && type <= Type::SealedGraffiti; }

        Type type;
        std::uint8_t rarity;
        WeaponId weaponID;
        std::size_t dataIndex;

        std::string iconPath;
    };

    struct PaintKit {
        PaintKit(int id, std::wstring&& name) noexcept;
        PaintKit(int id, std::wstring&& name, float wearRemapMin, float wearRemapMax) noexcept;
        PaintKit(int id, std::wstring&& name, std::uint32_t tournamentID, TournamentTeam tournamentTeam, int tournamentPlayerID, bool isGoldenSticker) noexcept;

        int id;
        float wearRemapMin = 0.0f;
        float wearRemapMax = 1.0f;
        std::uint32_t tournamentID = 0;
        TournamentTeam tournamentTeam{};
        bool isGoldenSticker = false;
        int tournamentPlayerID = 0;
        std::string name;
        std::wstring nameUpperCase;
    };

    enum class TournamentMap : std::uint8_t {
        None = 0,
        Cache,
        Cobblestone,
        Dust2,
        Inferno,
        Mirage,
        Nuke,
        Overpass,
        Train,
        Vertigo
    };

    struct Case {
        bool willProduceStatTrak = false;
        TournamentMap tournamentMap = TournamentMap::None;
        std::uint32_t souvenirPackageTournamentID = 0;
        std::size_t lootBeginIdx = 0;
        std::size_t lootEndIdx = 0;

        bool hasLoot() const noexcept { return lootEndIdx > lootBeginIdx; }
        bool isSouvenirPackage() const noexcept { return souvenirPackageTournamentID != 0; }
    };

    const std::vector<GameItem>& gameItems() noexcept;
    const std::vector<Case>& cases() noexcept;
    const std::vector<std::size_t>& caseLoot() noexcept;
    const std::vector<PaintKit>& paintKits() noexcept;
    std::wstring_view getWeaponNameUpper(WeaponId weaponID) noexcept;
    std::string_view getWeaponName(WeaponId weaponID) noexcept;

    std::size_t getItemIndex(WeaponId weaponID, int paintKit) noexcept;

    int findSouvenirTournamentSticker(std::uint32_t tournamentID) noexcept;
    int getTournamentTeamGoldStickerID(std::uint32_t tournamentID, TournamentTeam team) noexcept;
    int getTournamentPlayerGoldStickerID(std::uint32_t tournamentID, int tournamentPlayerID) noexcept;
    bool isCollectibleGenuine(const GameItem& collectible) noexcept;
    std::uint16_t getServiceMedalYear(const GameItem& serviceMedal) noexcept;
}
