#pragma once

#include <string_view>
#include <vector>

#include "../SDK/ItemSchema.h"
#include "StaticData.h"

class StaticDataStorage {
public:
    void addPatch(int id, StaticData::ItemName name, EconRarity rarity, std::string_view inventoryImage)
    {
        patchKits.emplace_back(id, name);
        addItem(game_items::Item::patch(rarity, patchKits.size() - 1, inventoryImage));
    }

    void addGraffiti(int id, StaticData::ItemName name, EconRarity rarity, std::string_view inventoryImage)
    {
        graffitiKits.emplace_back(id, name);
        const auto index = graffitiKits.size() - 1;
        addItem(game_items::Item::graffiti(rarity, index, inventoryImage));
        addItem(game_items::Item::sealedGraffiti(rarity, index, inventoryImage));
    }

    void addSticker(int id, StaticData::ItemName name, EconRarity rarity, std::string_view inventoryImage, std::uint32_t tournamentID, TournamentTeam tournamentTeam, int tournamentPlayerID, bool isGoldenSticker)
    {
        stickerKits.emplace_back(id, name, tournamentID, tournamentTeam, tournamentPlayerID, isGoldenSticker);
        addItem(game_items::Item::sticker(rarity, stickerKits.size() - 1, inventoryImage));
    }

    void addMusic(int musicID, StaticData::ItemName name, std::string_view inventoryImage)
    {
        musicKits.emplace_back(musicID, name);
        addItem(game_items::Item::musicKit(EconRarity::Blue, musicKits.size() - 1, inventoryImage));
    }

    void addVanillaKnife(WeaponId weaponID, std::string_view inventoryImage)
    {
        addItem(game_items::Item::skin(EconRarity::Red, weaponID, vanillaPaintIndex, inventoryImage));
    }

    void addCollectible(EconRarity rarity, WeaponId weaponID, bool isOriginal, std::string_view inventoryImage)
    {
        addItem(game_items::Item::collectible(rarity, weaponID, static_cast<std::size_t>(isOriginal), inventoryImage));
    }

    void addVanillaSkin(WeaponId weaponID, std::string_view inventoryImage)
    {
        addItem(game_items::Item::skin(EconRarity::Default, weaponID, vanillaPaintIndex, inventoryImage));
    }

    void addServiceMedal(EconRarity rarity, std::uint32_t year, WeaponId weaponID, std::string_view inventoryImage)
    {
        addItem(game_items::Item::serviceMedal(rarity, weaponID, static_cast<std::size_t>(year), inventoryImage));
    }

    void addTournamentCoin(EconRarity rarity, WeaponId weaponID, std::uint32_t tournamentEventID, std::string_view iconPath)
    {
        addItem(game_items::Item::tournamentCoin(rarity, weaponID, static_cast<std::size_t>(tournamentEventID), iconPath));
    }

    void addPaintKit(int id, StaticData::ItemName name, float wearRemapMin, float wearRemapMax)
    {
        paintKits.emplace_back(id, name, wearRemapMin, wearRemapMax);
    }

    void addGlovesWithLastPaintKit(EconRarity rarity, WeaponId weaponID, std::string_view iconPath)
    {
        addItem(game_items::Item::gloves(rarity, weaponID, paintKits.size() - 1, iconPath));
    }

    void addSkinWithLastPaintKit(EconRarity rarity, WeaponId weaponID, std::string_view iconPath)
    {
        addItem(game_items::Item::skin(rarity, weaponID, paintKits.size() - 1, iconPath));
    }

    void addNameTag(EconRarity rarity, WeaponId weaponID, std::string_view iconPath)
    {
        addItem(game_items::Item::nameTag(rarity, weaponID, 0, iconPath));
    }

    void addAgent(EconRarity rarity, WeaponId weaponID, std::string_view iconPath)
    {
        addItem(game_items::Item::agent(rarity, weaponID, 0, iconPath));
    }

    void addCase(EconRarity rarity, WeaponId weaponID, std::size_t descriptorIndex, std::string_view iconPath)
    {
        addItem(game_items::Item::crate(rarity, weaponID, descriptorIndex, iconPath));
    }

    void addCaseKey(EconRarity rarity, WeaponId weaponID, std::string_view iconPath)
    {
        addItem(game_items::Item::caseKey(rarity, weaponID, 0, iconPath));
    }

    void addOperationPass(EconRarity rarity, WeaponId weaponID, std::string_view iconPath)
    {
        addItem(game_items::Item::operationPass(rarity, weaponID, 0, iconPath));
    }

    void addStatTrakSwapTool(EconRarity rarity, WeaponId weaponID, std::string_view iconPath)
    {
        addItem(game_items::Item::statTrakSwapTool(rarity, weaponID, 0, iconPath));
    }

    void addSouvenirToken(EconRarity rarity, WeaponId weaponID, std::uint32_t tournamentEventID, std::string_view iconPath)
    {
        addItem(game_items::Item::souvenirToken(rarity, weaponID, static_cast<std::size_t>(tournamentEventID), iconPath));
    }

    void addViewerPass(EconRarity rarity, WeaponId weaponID, std::uint32_t tournamentEventID, std::string_view iconPath)
    {
        addItem(game_items::Item::viewerPass(rarity, weaponID, static_cast<std::size_t>(tournamentEventID), iconPath));
    }

    const auto& getStickerKit(const game_items::Item& item) const
    {
        assert(item.isSticker());
        return stickerKits[item.dataIndex];
    }

    const auto& getMusicKit(const game_items::Item& item) const
    {
        assert(item.isMusic());
        return musicKits[item.dataIndex];
    }

    const auto& getPaintKit(const game_items::Item& item) const
    {
        assert(item.isSkin() || item.isGloves());
        return paintKits[item.dataIndex];
    }

    const auto& getGraffitiKit(const game_items::Item& item) const
    {
        assert(item.isGraffiti() || item.isSealedGraffiti());
        return graffitiKits[item.dataIndex];
    }

    const auto& getPatchKit(const game_items::Item& item) const
    {
        assert(item.isPatch());
        return patchKits[item.dataIndex];
    }

    auto& getGameItems()
    {
        return gameItems;
    }

    const auto& getGameItems() const
    {
        return gameItems;
    }

    [[nodiscard]] std::uint16_t getServiceMedalYear(const game_items::Item& serviceMedal) const noexcept
    {
        assert(serviceMedal.isServiceMedal());
        return static_cast<std::uint16_t>(serviceMedal.dataIndex);
    }

    [[nodiscard]] bool isCollectibleGenuine(const game_items::Item& collectible) const noexcept
    {
        assert(collectible.isCollectible());
        return static_cast<bool>(collectible.dataIndex);
    }

private:
    void addItem(const game_items::Item& item)
    {
        gameItems.push_back(item);
    }

    static constexpr auto vanillaPaintIndex = 0;
    std::vector<StaticData::PaintKit> paintKits{ { 0, { "", L"" }, 0.0f, 1.0f } };
    std::vector<StaticData::StickerKit> stickerKits;
    std::vector<StaticData::MusicKit> musicKits;
    std::vector<StaticData::GraffitiKit> graffitiKits;
    std::vector<StaticData::PatchKit> patchKits;
    std::vector<game_items::Item> gameItems;
};
