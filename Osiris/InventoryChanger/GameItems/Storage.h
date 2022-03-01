#pragma once

#include <string_view>
#include <vector>

#include "Item.h"

#include "../StaticData.h"

namespace game_items
{

class Storage {
public:
    void addPatch(int id, ItemName name, EconRarity rarity, std::string_view inventoryImage)
    {
        patchKits.emplace_back(id, name);
        addItem(Item::patch(rarity, patchKits.size() - 1, inventoryImage));
    }

    void addGraffiti(int id, ItemName name, EconRarity rarity, std::string_view inventoryImage)
    {
        graffitiKits.emplace_back(id, name);
        const auto index = graffitiKits.size() - 1;
        addItem(Item::graffiti(rarity, index, inventoryImage));
        addItem(Item::sealedGraffiti(rarity, index, inventoryImage));
    }

    void addSticker(int id, ItemName name, EconRarity rarity, std::string_view inventoryImage, std::uint32_t tournamentID, TournamentTeam tournamentTeam, int tournamentPlayerID, bool isGoldenSticker)
    {
        stickerKits.emplace_back(id, name, tournamentID, tournamentTeam, tournamentPlayerID, isGoldenSticker);
        addItem(Item::sticker(rarity, stickerKits.size() - 1, inventoryImage));
    }

    void addMusic(int musicID, ItemName name, std::string_view inventoryImage)
    {
        musicKits.emplace_back(musicID, name);
        addItem(Item::musicKit(EconRarity::Blue, musicKits.size() - 1, inventoryImage));
    }

    void addVanillaKnife(WeaponId weaponID, std::string_view inventoryImage)
    {
        addItem(Item::skin(EconRarity::Red, weaponID, vanillaPaintIndex, inventoryImage));
    }

    void addCollectible(EconRarity rarity, WeaponId weaponID, bool isOriginal, std::string_view inventoryImage)
    {
        addItem(Item::collectible(rarity, weaponID, static_cast<std::size_t>(isOriginal), inventoryImage));
    }

    void addVanillaSkin(WeaponId weaponID, std::string_view inventoryImage)
    {
        addItem(Item::skin(EconRarity::Default, weaponID, vanillaPaintIndex, inventoryImage));
    }

    void addServiceMedal(EconRarity rarity, std::uint32_t year, WeaponId weaponID, std::string_view inventoryImage)
    {
        addItem(Item::serviceMedal(rarity, weaponID, static_cast<std::size_t>(year), inventoryImage));
    }

    void addTournamentCoin(EconRarity rarity, WeaponId weaponID, std::uint32_t tournamentEventID, std::string_view iconPath)
    {
        addItem(Item::tournamentCoin(rarity, weaponID, static_cast<std::size_t>(tournamentEventID), iconPath));
    }

    void addPaintKit(int id, ItemName name, float wearRemapMin, float wearRemapMax)
    {
        paintKits.emplace_back(id, name, wearRemapMin, wearRemapMax);
    }

    void addGlovesWithLastPaintKit(EconRarity rarity, WeaponId weaponID, std::string_view iconPath)
    {
        addItem(Item::gloves(rarity, weaponID, paintKits.size() - 1, iconPath));
    }

    void addSkinWithLastPaintKit(EconRarity rarity, WeaponId weaponID, std::string_view iconPath)
    {
        addItem(Item::skin(rarity, weaponID, paintKits.size() - 1, iconPath));
    }

    void addNameTag(EconRarity rarity, WeaponId weaponID, std::string_view iconPath)
    {
        addItem(Item::nameTag(rarity, weaponID, 0, iconPath));
    }

    void addAgent(EconRarity rarity, WeaponId weaponID, std::string_view iconPath)
    {
        addItem(Item::agent(rarity, weaponID, 0, iconPath));
    }

    void addCase(EconRarity rarity, WeaponId weaponID, std::size_t descriptorIndex, std::string_view iconPath)
    {
        addItem(Item::crate(rarity, weaponID, descriptorIndex, iconPath));
    }

    void addCaseKey(EconRarity rarity, WeaponId weaponID, std::string_view iconPath)
    {
        addItem(Item::caseKey(rarity, weaponID, 0, iconPath));
    }

    void addOperationPass(EconRarity rarity, WeaponId weaponID, std::string_view iconPath)
    {
        addItem(Item::operationPass(rarity, weaponID, 0, iconPath));
    }

    void addStatTrakSwapTool(EconRarity rarity, WeaponId weaponID, std::string_view iconPath)
    {
        addItem(Item::statTrakSwapTool(rarity, weaponID, 0, iconPath));
    }

    void addSouvenirToken(EconRarity rarity, WeaponId weaponID, std::uint32_t tournamentEventID, std::string_view iconPath)
    {
        addItem(Item::souvenirToken(rarity, weaponID, static_cast<std::size_t>(tournamentEventID), iconPath));
    }

    void addViewerPass(EconRarity rarity, WeaponId weaponID, std::uint32_t tournamentEventID, std::string_view iconPath)
    {
        addItem(Item::viewerPass(rarity, weaponID, static_cast<std::size_t>(tournamentEventID), iconPath));
    }

    const auto& getStickerKit(const Item& item) const
    {
        assert(item.isSticker());
        return stickerKits[item.getDataIndex()];
    }

    const auto& getMusicKit(const Item& item) const
    {
        assert(item.isMusic());
        return musicKits[item.getDataIndex()];
    }

    const auto& getPaintKit(const Item& item) const
    {
        assert(item.isSkin() || item.isGloves());
        return paintKits[item.getDataIndex()];
    }

    const auto& getGraffitiKit(const Item& item) const
    {
        assert(item.isGraffiti() || item.isSealedGraffiti());
        return graffitiKits[item.getDataIndex()];
    }

    const auto& getPatchKit(const Item& item) const
    {
        assert(item.isPatch());
        return patchKits[item.getDataIndex()];
    }

    auto& getGameItems()
    {
        return gameItems;
    }

    const auto& getGameItems() const
    {
        return gameItems;
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

    void compress()
    {
        paintKits.shrink_to_fit();
        stickerKits.shrink_to_fit();
        musicKits.shrink_to_fit();
        graffitiKits.shrink_to_fit();
        patchKits.shrink_to_fit();
        gameItems.shrink_to_fit();
    }

private:
    void addItem(const Item& item)
    {
        gameItems.push_back(item);
    }

    static constexpr auto vanillaPaintIndex = 0;
    std::vector<PaintKit> paintKits{ { 0, { "", L"" }, 0.0f, 1.0f } };
    std::vector<StickerKit> stickerKits;
    std::vector<MusicKit> musicKits;
    std::vector<GraffitiKit> graffitiKits;
    std::vector<PatchKit> patchKits;
    std::vector<Item> gameItems;
};

}
