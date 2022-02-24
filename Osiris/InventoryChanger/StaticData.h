#pragma once

#include <cassert>
#include <cstdint>
#include <functional>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include "../SDK/WeaponId.h"

enum TournamentTeam : std::uint8_t;
enum class EconRarity : std::uint8_t;

namespace StaticData
{
    struct ItemIndex2 {
        explicit ItemIndex2(std::size_t value) : value{ value } {}
        ItemIndex2() = default;
        friend bool operator==(const ItemIndex2& a, const ItemIndex2& b) = default;

        std::size_t value = static_cast<std::size_t>(-1);
    };

    constexpr auto InvalidItemIdx2 = ItemIndex2{};

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

        [[nodiscard]] static constexpr GameItem case_(EconRarity rarity, WeaponId weaponID, std::size_t dataIndex, std::string_view iconPath) noexcept
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

        bool hasPaintKit() const noexcept { return type >= Type::Gloves && type <= Type::Patch; }

    private:
        enum class Type : std::uint8_t {
            // has paint kit, must match GameItem::hasPaintKit() below
            Gloves,
            Skin,
            Patch,

            // has other data
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

    struct ItemName {
        std::string_view forDisplay;
        std::wstring_view forSearch;
    };

    struct MusicKit {
        MusicKit(int id, ItemName name) : id{ id }, name{ name } {}

        int id;
        ItemName name;
    };

    struct GraffitiKit {
        GraffitiKit(int id, ItemName name) : id{ id }, name{ name } {}

        int id;
        ItemName name;
    };

    struct StickerKit {
        StickerKit(int id, ItemName name, std::uint32_t tournamentID, TournamentTeam tournamentTeam, int tournamentPlayerID, bool isGoldenSticker)
            : id{ id }, name{ name }, tournamentID{ tournamentID }, tournamentTeam{ tournamentTeam }, isGoldenSticker{ isGoldenSticker }, tournamentPlayerID{ tournamentPlayerID } {}

        int id;
        ItemName name;
        std::uint32_t tournamentID = 0;
        TournamentTeam tournamentTeam{};
        bool isGoldenSticker = false;
        int tournamentPlayerID = 0;
    };

    struct PaintKit {
        PaintKit(int id, ItemName name) noexcept;
        PaintKit(int id, ItemName name, float wearRemapMin, float wearRemapMax) noexcept;

        int id;
        float wearRemapMin = 0.0f;
        float wearRemapMax = 1.0f;
        ItemName name;
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
        std::size_t lootBeginIdx;
        std::size_t lootEndIdx;

        bool hasLoot() const noexcept { return lootEndIdx > lootBeginIdx; }
        bool isSouvenirPackage() const noexcept { return souvenirPackageTournamentID != 0; }
    };

    [[nodiscard]] std::size_t getGameItemsCount() noexcept;
    const std::vector<std::reference_wrapper<const GameItem>>& caseLoot() noexcept;
    [[nodiscard]] std::vector<ItemIndex2> getItemIndices() noexcept;
    
    [[nodiscard]] int getStickerID(const GameItem& item) noexcept;
    [[nodiscard]] int getMusicID(const GameItem& item) noexcept;
    [[nodiscard]] int getPatchID(const GameItem& item) noexcept;
    [[nodiscard]] int getSkinPaintID(const GameItem& item) noexcept;
    [[nodiscard]] int getGraffitiID(const GameItem& item) noexcept;
    [[nodiscard]] int getSealedGraffitiID(const GameItem& item) noexcept;
    
    [[nodiscard]] std::string_view getPaintName(const GameItem& item) noexcept;
    [[nodiscard]] std::wstring_view getPaintNameUpper(const GameItem& item) noexcept;
    [[nodiscard]] const PaintKit& getPaintKit(const GameItem& item) noexcept;
    [[nodiscard]] const Case& getCase(const GameItem& item) noexcept;
    [[nodiscard]] const GameItem& getGameItem(ItemIndex2 itemIndex) noexcept;

    std::wstring_view getWeaponNameUpper(WeaponId weaponID) noexcept;
    std::string_view getWeaponName(WeaponId weaponID) noexcept;

    ItemIndex2 getItemIndex(WeaponId weaponID, int paintKit) noexcept;
    [[nodiscard]] ItemIndex2 getItemIndex(WeaponId weaponID) noexcept;
    [[nodiscard]] std::optional<std::reference_wrapper<const GameItem>> getItem(WeaponId weaponID) noexcept;
    [[nodiscard]] std::optional<std::reference_wrapper<const GameItem>> getItemWithPaintkit(WeaponId weaponID, int paintKit) noexcept;
    [[nodiscard]] std::optional<std::reference_wrapper<const GameItem>> getGraffiti(int graffitiID) noexcept;
    [[nodiscard]] std::optional<std::reference_wrapper<const GameItem>> getSealedGraffiti(int graffitiID) noexcept;
    [[nodiscard]] std::optional<std::reference_wrapper<const GameItem>> getMusic(int musicID) noexcept;
    [[nodiscard]] std::optional<std::reference_wrapper<const GameItem>> getSticker(int stickerID) noexcept;
    ItemIndex2 getMusicIndex(int musicID) noexcept;
    ItemIndex2 getGraffitiIndex(int graffitiID) noexcept;
    ItemIndex2 getSealedGraffitiIndex(int graffitiID) noexcept;
    ItemIndex2 getStickerIndex(int stickerID) noexcept;

    int findSouvenirTournamentSticker(std::uint32_t tournamentID) noexcept;
    int getTournamentTeamGoldStickerID(std::uint32_t tournamentID, TournamentTeam team) noexcept;
    int getTournamentPlayerGoldStickerID(std::uint32_t tournamentID, int tournamentPlayerID) noexcept;
    int getTournamentMapGoldStickerID(TournamentMap map) noexcept;
    bool isCollectibleGenuine(const GameItem& collectible) noexcept;
    std::uint16_t getServiceMedalYear(const GameItem& serviceMedal) noexcept;
    [[nodiscard]] std::uint32_t getTournamentEventID(const GameItem& item) noexcept;

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

template <>
struct std::hash<StaticData::ItemIndex2> {
    std::size_t operator()(StaticData::ItemIndex2 s) const noexcept
    {
        return std::hash<std::size_t>{}(s.value);
    }
};
