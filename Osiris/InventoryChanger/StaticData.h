#pragma once

#include <bit>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>
#include "../SDK/ItemSchema.h"
#include "../SDK/WeaponId.h"

enum TournamentTeam : std::uint8_t;
enum class EconRarity : std::uint8_t;

namespace game_items
{
    struct Item;
    class Lookup;

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

    struct PatchKit {
        PatchKit(int id, ItemName name) : id{ id }, name{ name } {}

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
        PaintKit(int id, ItemName name, float wearRemapMin, float wearRemapMax) noexcept;

        int id;
        float wearRemapMin = 0.0f;
        float wearRemapMax = 1.0f;
        ItemName name;
    };
}

namespace StaticData
{
    struct ItemIndex2 {
        explicit ItemIndex2(std::size_t value) : value{ value } {}
        ItemIndex2() = default;
        friend bool operator==(const ItemIndex2& a, const ItemIndex2& b) = default;

        std::size_t value = static_cast<std::size_t>(-1);
    };

    constexpr auto InvalidItemIdx2 = ItemIndex2{};

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

    class EconRarities {
    public:
        EconRarities() = default;

        template <typename... Args>
        constexpr EconRarities(Args... args) noexcept
        {
            (set(args), ...);
        }

        constexpr void set(EconRarity rarity) noexcept
        {
            if (const auto rarityBit = static_cast<std::uint8_t>(rarity); rarityBit < std::numeric_limits<std::uint8_t>::digits)
                bits |= (std::byte{ 1 } << rarityBit);
        }

        [[nodiscard]] constexpr EconRarity getNthRarity(std::uint8_t n) const noexcept
        {
            for (std::uint8_t i = 0; i < std::numeric_limits<std::uint8_t>::digits; ++i) {
                if (std::to_integer<std::uint8_t>(bits & (std::byte{ 1 } << i)) != 0) {
                    if (n == 0)
                        return static_cast<EconRarity>(i);
                    --n;
                }
            }
            return EconRarity::Default;
        }

        [[nodiscard]] constexpr std::size_t count() const noexcept
        {
            return static_cast<std::size_t>(std::popcount(std::to_integer<std::uint8_t>(bits)));
        }

    private:
        std::byte bits{ 0 };
    };

    struct Case {
        bool willProduceStatTrak = false;
        EconRarities rarities{};
        TournamentMap tournamentMap = TournamentMap::None;
        std::uint32_t souvenirPackageTournamentID = 0;
        std::size_t lootBeginIdx;
        std::size_t lootEndIdx;

        bool hasLoot() const noexcept { return lootEndIdx > lootBeginIdx; }
        bool isSouvenirPackage() const noexcept { return souvenirPackageTournamentID != 0; }
    };

    [[nodiscard]] std::size_t getGameItemsCount() noexcept;
    const std::vector<std::reference_wrapper<const game_items::Item>>& caseLoot() noexcept;
    [[nodiscard]] std::vector<ItemIndex2> getItemIndices() noexcept;
    
    [[nodiscard]] int getStickerID(const game_items::Item& item) noexcept;
    [[nodiscard]] int getMusicID(const game_items::Item& item) noexcept;
    [[nodiscard]] int getPatchID(const game_items::Item& item) noexcept;
    [[nodiscard]] int getSkinPaintID(const game_items::Item& item) noexcept;
    [[nodiscard]] int getGraffitiID(const game_items::Item& item) noexcept;
    [[nodiscard]] int getSealedGraffitiID(const game_items::Item& item) noexcept;
    
    [[nodiscard]] std::string_view getPaintName(const game_items::Item& item) noexcept;
    [[nodiscard]] std::wstring_view getPaintNameUpper(const game_items::Item& item) noexcept;
    [[nodiscard]] const game_items::PaintKit& getPaintKit(const game_items::Item& item) noexcept;
    [[nodiscard]] const Case& getCase(const game_items::Item& item) noexcept;
    [[nodiscard]] const game_items::Item& getGameItem(ItemIndex2 itemIndex) noexcept;

    std::wstring_view getWeaponNameUpper(WeaponId weaponID) noexcept;
    std::string_view getWeaponName(WeaponId weaponID) noexcept;

    [[nodiscard]] const game_items::Lookup& lookup() noexcept;

    int findSouvenirTournamentSticker(std::uint32_t tournamentID) noexcept;
    int getTournamentTeamGoldStickerID(std::uint32_t tournamentID, TournamentTeam team) noexcept;
    int getTournamentPlayerGoldStickerID(std::uint32_t tournamentID, int tournamentPlayerID) noexcept;
    int getTournamentMapGoldStickerID(TournamentMap map) noexcept;
    bool isCollectibleGenuine(const game_items::Item& collectible) noexcept;
    std::uint16_t getServiceMedalYear(const game_items::Item& serviceMedal) noexcept;
    [[nodiscard]] std::uint32_t getTournamentEventID(const game_items::Item& item) noexcept;

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
