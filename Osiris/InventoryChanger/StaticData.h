#pragma once

#include <cassert>
#include <cstdint>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include "../SDK/WeaponId.h"

enum TournamentTeam : std::uint8_t;

namespace StaticData
{
    using ItemIndex = std::size_t;
    constexpr auto InvalidItemIdx = static_cast<ItemIndex>(-1);

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
        ServiceMedal,
        SouvenirToken,
        TournamentCoin
    };

    struct GameItem {
        GameItem(Type type, int rarity, WeaponId weaponID, std::size_t dataIndex, std::string_view iconPath) noexcept;

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
        bool isSouvenirToken() const noexcept { return type == Type::SouvenirToken; }
        bool isTournamentCoin() const noexcept { return type == Type::TournamentCoin; }

        bool hasPaintKit() const noexcept { return type >= Type::Sticker && type <= Type::SealedGraffiti && type != Type::Music; }

        int tournamentEventID() const noexcept { assert(isTournamentCoin()); return static_cast<int>(dataIndex); }

        Type type;
        std::uint8_t rarity;
        WeaponId weaponID;
        std::size_t dataIndex;

        std::string_view iconPath;
    };

    struct MusicKit {
        MusicKit(int id, std::string_view name, std::wstring_view nameUpperCase) : id{ id }, name{ name }, nameUpperCase{ nameUpperCase } {}

        int id;
        std::string_view name;
        std::wstring_view nameUpperCase;
    };

    struct StickerKit {
        StickerKit(int id, std::string name, std::wstring nameUpperCase, std::uint32_t tournamentID, TournamentTeam tournamentTeam, int tournamentPlayerID, bool isGoldenSticker)
            : id{ id }, name{ name }, nameUpperCase{ nameUpperCase }, tournamentID{ tournamentID }, tournamentTeam{ tournamentTeam }, isGoldenSticker{ isGoldenSticker }, tournamentPlayerID{ tournamentPlayerID } {}

        int id;
        std::string name;
        std::wstring nameUpperCase;
        std::uint32_t tournamentID = 0;
        TournamentTeam tournamentTeam{};
        bool isGoldenSticker = false;
        int tournamentPlayerID = 0;
    };

    struct PaintKit {
        PaintKit(int id, std::string name, std::wstring nameUpperCase) noexcept;
        PaintKit(int id, std::string name, std::wstring nameUpperCase, float wearRemapMin, float wearRemapMax) noexcept;
        PaintKit(int id, std::string name, std::wstring nameUpperCase, std::uint32_t tournamentID, TournamentTeam tournamentTeam, int tournamentPlayerID, bool isGoldenSticker) noexcept;

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
        Ancient,
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
        ItemIndex lootBeginIdx = 0;
        ItemIndex lootEndIdx = 0;

        bool hasLoot() const noexcept { return lootEndIdx > lootBeginIdx; }
        bool isSouvenirPackage() const noexcept { return souvenirPackageTournamentID != 0; }
    };

    std::span<const GameItem> gameItems() noexcept;
    const std::vector<ItemIndex>& caseLoot() noexcept;
    
    [[nodiscard]] int getStickerID(const GameItem& item) noexcept;
    [[nodiscard]] int getMusicID(const GameItem& item) noexcept;
    [[nodiscard]] int getPatchID(const GameItem& item) noexcept;
    [[nodiscard]] int getSkinPaintID(const GameItem& item) noexcept;
    
    [[nodiscard]] std::string_view getPaintName(const GameItem& item) noexcept;
    [[nodiscard]] const PaintKit& getPaintKit(const GameItem& item) noexcept;
    [[nodiscard]] const Case& getCase(const GameItem& item) noexcept;
    [[nodiscard]] const GameItem& getGameItem(ItemIndex itemIndex) noexcept;

    std::wstring_view getWeaponNameUpper(WeaponId weaponID) noexcept;
    std::string_view getWeaponName(WeaponId weaponID) noexcept;

    ItemIndex getItemIndex(WeaponId weaponID, int paintKit) noexcept;

    int findSouvenirTournamentSticker(std::uint32_t tournamentID) noexcept;
    int getTournamentTeamGoldStickerID(std::uint32_t tournamentID, TournamentTeam team) noexcept;
    int getTournamentPlayerGoldStickerID(std::uint32_t tournamentID, int tournamentPlayerID) noexcept;
    int getTournamentMapGoldStickerID(TournamentMap map) noexcept;
    bool isCollectibleGenuine(const GameItem& collectible) noexcept;
    std::uint16_t getServiceMedalYear(const GameItem& serviceMedal) noexcept;

    constexpr TournamentMap getTournamentMapOfSouvenirPackage(std::string_view lootListName) noexcept
    {
        if (lootListName.ends_with("de_dust2"))
            return TournamentMap::Dust2;
        if (lootListName.ends_with("de_mirage"))
            return TournamentMap::Mirage;
        if (lootListName.ends_with("de_inferno"))
            return TournamentMap::Inferno;
        if (lootListName.ends_with("de_cbble"))
            return TournamentMap::Cobblestone;
        if (lootListName.ends_with("de_overpass"))
            return TournamentMap::Overpass;
        if (lootListName.ends_with("de_cache"))
            return TournamentMap::Cache;
        if (lootListName.ends_with("de_train"))
            return TournamentMap::Train;
        if (lootListName.ends_with("de_nuke"))
            return TournamentMap::Nuke;
        if (lootListName.ends_with("de_vertigo"))
            return TournamentMap::Vertigo;
        if (lootListName.ends_with("de_ancient"))
            return TournamentMap::Ancient;
        return TournamentMap::None;
    }
}
