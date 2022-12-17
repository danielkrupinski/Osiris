#include "Storage.h"

#include <CSGO/ItemSchema.h>
#include <CSGO/WeaponId.h>

namespace inventory_changer::game_items
{

void Storage::addPatch(int id, ItemName name, EconRarity rarity, std::string_view iconPath)
{
    patches.emplace_back(id, pooled(name));
    addItem(Item::Type::Patch, rarity, WeaponId::Patch, patches.size() - 1, pooled(iconPath));
}

void Storage::addGraffiti(int id, ItemName name, EconRarity rarity, std::string_view iconPath)
{
    graffitiKits.emplace_back(id, pooled(name));
    const auto index = graffitiKits.size() - 1;
    addItem(Item::Type::Graffiti, rarity, WeaponId::SealedGraffiti, index, pooled(iconPath));
}

void Storage::addSticker(csgo::StickerId id, ItemName name, EconRarity rarity, std::string_view iconPath, csgo::Tournament tournament, csgo::TournamentTeam tournamentTeam, int tournamentPlayerID, bool isGoldenSticker)
{
    stickerKits.emplace_back(id, pooled(name), tournament, tournamentTeam, tournamentPlayerID, isGoldenSticker);
    addItem(Item::Type::Sticker, rarity, WeaponId::Sticker, stickerKits.size() - 1, pooled(iconPath));
}

void Storage::addMusic(int musicID, ItemName name, std::string_view iconPath)
{
    musicKits.emplace_back(musicID, pooled(name));
    addItem(Item::Type::Music, EconRarity::Blue, WeaponId::MusicKit, musicKits.size() - 1, pooled(iconPath));
}

void Storage::addVanillaKnife(WeaponId weaponID, std::string_view iconPath)
{
    addItem(Item::Type::Skin, EconRarity::Red, weaponID, vanillaPaintIndex, pooled(iconPath));
}

void Storage::addCollectible(EconRarity rarity, WeaponId weaponID, bool isOriginal, std::string_view iconPath)
{
    addItem(Item::Type::Collectible, rarity, weaponID, static_cast<std::size_t>(isOriginal), pooled(iconPath));
}

void Storage::addVanillaSkin(WeaponId weaponID, std::string_view iconPath)
{
    addItem(Item::Type::Skin, EconRarity::Default, weaponID, vanillaPaintIndex, pooled(iconPath));
}

void Storage::addServiceMedal(EconRarity rarity, std::uint32_t year, WeaponId weaponID, std::string_view iconPath)
{
    addItem(Item::Type::ServiceMedal, rarity, weaponID, static_cast<std::size_t>(year), pooled(iconPath));
}

void Storage::addTournamentCoin(EconRarity rarity, WeaponId weaponID, csgo::Tournament tournament, std::uint16_t defaultGraffitiID, std::string_view iconPath)
{
    addItem(Item::Type::TournamentCoin, rarity, weaponID, static_cast<std::size_t>(static_cast<std::uint8_t>(tournament) | defaultGraffitiID << 8), pooled(iconPath));
}

void Storage::addPaintKit(int id, ItemName name, float wearRemapMin, float wearRemapMax)
{
    paintKits.emplace_back(id, pooled(name), wearRemapMin, wearRemapMax);
}

void Storage::addGlovesWithLastPaintKit(EconRarity rarity, WeaponId weaponID, std::string_view iconPath)
{
    addItem(Item::Type::Gloves, rarity, weaponID, paintKits.size() - 1, pooled(iconPath));
}

void Storage::addSkinWithLastPaintKit(EconRarity rarity, WeaponId weaponID, std::string_view iconPath)
{
    addItem(Item::Type::Skin, rarity, weaponID, paintKits.size() - 1, pooled(iconPath));
}

void Storage::addNameTag(EconRarity rarity, WeaponId weaponID, std::string_view iconPath)
{
    addItem(Item::Type::NameTag, rarity, weaponID, 0, pooled(iconPath));
}

void Storage::addAgent(EconRarity rarity, WeaponId weaponID, std::string_view iconPath)
{
    addItem(Item::Type::Agent, rarity, weaponID, 0, pooled(iconPath));
}

void Storage::addCrate(EconRarity rarity, WeaponId weaponID, std::uint16_t crateSeries, csgo::Tournament tournament, TournamentMap map, bool isSouvenirPackage, std::string_view iconPath)
{
    assert((static_cast<std::uint8_t>(map) & 0x80) == 0);
    addItem(Item::Type::Crate, rarity, weaponID, (static_cast<std::uint8_t>(isSouvenirPackage) << 31) | (static_cast<std::uint8_t>(map) & 0x7F) << 24 | crateSeries << 8 | static_cast<std::uint8_t>(tournament), pooled(iconPath));
}

void Storage::addCaseKey(EconRarity rarity, WeaponId weaponID, std::string_view iconPath)
{
    addItem(Item::Type::CrateKey, rarity, weaponID, 0, pooled(iconPath));
}

void Storage::addOperationPass(EconRarity rarity, WeaponId weaponID, std::string_view iconPath)
{
    addItem(Item::Type::OperationPass, rarity, weaponID, 0, pooled(iconPath));
}

void Storage::addStatTrakSwapTool(EconRarity rarity, WeaponId weaponID, std::string_view iconPath)
{
    addItem(Item::Type::StatTrakSwapTool, rarity, weaponID, 0, pooled(iconPath));
}

void Storage::addSouvenirToken(EconRarity rarity, WeaponId weaponID, csgo::Tournament tournament, std::string_view iconPath)
{
    addItem(Item::Type::SouvenirToken, rarity, weaponID, static_cast<std::size_t>(tournament), pooled(iconPath));
}

void Storage::addViewerPass(EconRarity rarity, WeaponId weaponID, csgo::Tournament tournament, bool hasExtraTokens, std::string_view iconPath)
{
    addItem(Item::Type::ViewerPass, rarity, weaponID, static_cast<std::size_t>(tournament) | (static_cast<std::size_t>(hasExtraTokens) << 8), pooled(iconPath));
}

void Storage::addStorageUnit(EconRarity rarity, WeaponId weaponID, std::string_view iconPath)
{
    addItem(Item::Type::StorageUnit, rarity, weaponID, 0, pooled(iconPath));
}

void Storage::compress()
{
    paintKits.shrink_to_fit();
    stickerKits.shrink_to_fit();
    musicKits.shrink_to_fit();
    graffitiKits.shrink_to_fit();
    patches.shrink_to_fit();
    items.shrink_to_fit();
}

std::string_view Storage::pooled(std::string_view string)
{
    return stringPool.add(string);
}

ItemName Storage::pooled(const ItemName& name)
{
    return ItemName{ stringPool.add(name.forDisplay), stringPoolWide.add(name.forSearch) };
}

const ItemName& getItemName(const Storage& gameItemStorage, const Item& item)
{
    if (item.isSkin() || item.isGloves())
        return gameItemStorage.getPaintKit(item).name;
    if (item.isMusic())
        return gameItemStorage.getMusicKit(item).name;
    if (item.isSticker())
        return gameItemStorage.getStickerKit(item).name;
    if (item.isGraffiti())
        return gameItemStorage.getGraffitiKit(item).name;
    if (item.isPatch())
        return gameItemStorage.getPatch(item).name;

    static constexpr ItemName fallback{ "", L"" };
    return fallback;
}

}
