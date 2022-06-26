#pragma once

#include <span>
#include <string_view>
#include <utility>
#include <vector>

#include <StringPool.h>

#include "Item.h"
#include "Structs.h"

namespace inventory_changer
{
    enum class TournamentMap : std::uint8_t;
}

namespace inventory_changer::game_items
{

class Storage {
public:
    void addPatch(int id, ItemName name, EconRarity rarity, std::string_view iconPath);
    void addGraffiti(int id, ItemName name, EconRarity rarity, std::string_view iconPath);
    void addSticker(int id, ItemName name, EconRarity rarity, std::string_view iconPath, std::uint32_t tournamentID, TournamentTeam tournamentTeam, int tournamentPlayerID, bool isGoldenSticker);
    void addMusic(int musicID, ItemName name, std::string_view iconPath);
    void addVanillaKnife(WeaponId weaponID, std::string_view iconPath);
    void addCollectible(EconRarity rarity, WeaponId weaponID, bool isOriginal, std::string_view iconPath);
    void addVanillaSkin(WeaponId weaponID, std::string_view iconPath);
    void addServiceMedal(EconRarity rarity, std::uint32_t year, WeaponId weaponID, std::string_view iconPath);
    void addTournamentCoin(EconRarity rarity, WeaponId weaponID, std::uint8_t tournamentEventID, std::uint16_t defaultGraffitiID, std::string_view iconPath);
    void addPaintKit(int id, ItemName name, float wearRemapMin, float wearRemapMax);
    void addGlovesWithLastPaintKit(EconRarity rarity, WeaponId weaponID, std::string_view iconPath);
    void addSkinWithLastPaintKit(EconRarity rarity, WeaponId weaponID, std::string_view iconPath);
    void addNameTag(EconRarity rarity, WeaponId weaponID, std::string_view iconPath);
    void addAgent(EconRarity rarity, WeaponId weaponID, std::string_view iconPath);
    void addCase(EconRarity rarity, WeaponId weaponID, std::uint16_t crateSeries, std::uint8_t tournamentID, TournamentMap map, bool isSouvenirPackage, std::string_view iconPath);
    void addCaseKey(EconRarity rarity, WeaponId weaponID, std::string_view iconPath);
    void addOperationPass(EconRarity rarity, WeaponId weaponID, std::string_view iconPath);
    void addStatTrakSwapTool(EconRarity rarity, WeaponId weaponID, std::string_view iconPath);
    void addSouvenirToken(EconRarity rarity, WeaponId weaponID, std::uint8_t tournamentEventID, std::string_view iconPath);
    void addViewerPass(EconRarity rarity, WeaponId weaponID, std::uint8_t tournamentEventID, std::string_view iconPath);

    [[nodiscard]] const auto& getStickerKit(const Item& item) const
    {
        assert(item.isSticker());
        return stickerKits[item.getDataIndex()];
    }

    [[nodiscard]] const auto& getMusicKit(const Item& item) const
    {
        assert(item.isMusic());
        return musicKits[item.getDataIndex()];
    }

    [[nodiscard]] const auto& getPaintKit(const Item& item) const
    {
        assert(item.isSkin() || item.isGloves());
        return paintKits[item.getDataIndex()];
    }

    [[nodiscard]] const auto& getGraffitiKit(const Item& item) const
    {
        assert(item.isGraffiti());
        return graffitiKits[item.getDataIndex()];
    }

    [[nodiscard]] const auto& getPatch(const Item& item) const
    {
        assert(item.isPatch());
        return patches[item.getDataIndex()];
    }

    [[nodiscard]] std::span<Item> getItems()
    {
        return items;
    }

    [[nodiscard]] std::span<const Item> getItems() const
    {
        return items;
    }

    [[nodiscard]] std::span<const PaintKit> getPaintKits() const
    {
        return paintKits;
    }

    [[nodiscard]] std::uint16_t getServiceMedalYear(const Item& serviceMedal) const noexcept
    {
        assert(serviceMedal.isServiceMedal());
        return static_cast<std::uint16_t>(serviceMedal.getDataIndex());
    }

    [[nodiscard]] bool isCollectibleGenuine(const Item& collectible) const noexcept
    {
        assert(collectible.isCollectible());
        return static_cast<bool>(collectible.getDataIndex());
    }

    [[nodiscard]] std::uint8_t getTournamentEventID(const Item& item) const noexcept
    {
        assert(item.isSouvenirToken() || item.isViewerPass() || item.isTournamentCoin() || item.isCase());
        return static_cast<std::uint8_t>(item.getDataIndex() & 0xFF);
    }

    [[nodiscard]] std::uint16_t getDefaultTournamentGraffitiID(const Item& item) const noexcept
    {
        assert(item.isTournamentCoin());
        return static_cast<std::uint16_t>(item.getDataIndex() >> 8);
    }

    [[nodiscard]] std::uint16_t getCrateSeries(const Item& crate) const noexcept
    {
        assert(crate.isCase());
        return static_cast<std::uint16_t>(crate.getDataIndex() >> 8);
    }

    [[nodiscard]] TournamentMap getTournamentMap(const Item& crate) const noexcept
    {
        assert(crate.isCase());
        return static_cast<TournamentMap>((crate.getDataIndex() >> 24) & 0x7F);
    }

    [[nodiscard]] bool isSouvenirPackage(const Item& crate) const noexcept
    {
        assert(crate.isCase());
        return ((crate.getDataIndex() >> 31) & 1) != 0;
    }

    [[nodiscard]] bool hasPaintKit(const Item& item) const noexcept
    {
        return item.isSkin() || item.isGloves();
    }

    void compress();

private:
    template <typename... Args>
    void addItem(Args&&... args)
    {
        items.emplace_back(std::forward<Args>(args)...);
    }

    [[nodiscard]] std::string_view pooled(std::string_view string);
    [[nodiscard]] ItemName pooled(const ItemName& name);

    static constexpr auto vanillaPaintIndex = 0;

    StringPool<char> stringPool;
    StringPool<wchar_t> stringPoolWide;
    std::vector<PaintKit> paintKits{ { 0, { "", L"" }, 0.0f, 1.0f } };
    std::vector<StickerKit> stickerKits;
    std::vector<MusicKit> musicKits;
    std::vector<GraffitiKit> graffitiKits;
    std::vector<Patch> patches;
    std::vector<Item> items;
};

[[nodiscard]] const ItemName& getItemName(const Storage& gameItemStorage, const Item& item);

}
