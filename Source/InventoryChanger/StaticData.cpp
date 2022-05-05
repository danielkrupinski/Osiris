#include <algorithm>
#include <cstring>
#include <numeric>
#include <optional>
#include <string_view>
#include <unordered_map>
#include <utility>

#include <range/v3/algorithm/equal_range.hpp>

#include "StaticData.h"

#include "../Helpers.h"
#include "../Interfaces.h"
#include "../SDK/ItemSchema.h"
#include "../SDK/Localize.h"
#include <StringPool.h>
#include "GameItems/Lookup.h"

using StaticData::TournamentMap;

constexpr auto operator<=>(WeaponId a, WeaponId b) noexcept
{
    return static_cast<std::underlying_type_t<WeaponId>>(a) <=> static_cast<std::underlying_type_t<WeaponId>>(b);
}

class WeaponNames {
public:
    [[nodiscard]] std::string_view getWeaponName(WeaponId weaponID) const
    {
        if (const auto it = names.find(weaponID); it != names.end())
            return it->second.first;
        return "";
    }

    [[nodiscard]] std::wstring_view getWeaponNameUpper(WeaponId weaponID) const
    {
        if (const auto it = names.find(weaponID); it != names.end())
            return it->second.second;
        return L"";
    }

    void add(WeaponId weaponID, std::string_view name, std::wstring_view nameUpperCase)
    {
        names.emplace(weaponID, std::make_pair(name, nameUpperCase));
    }

private:
    std::unordered_map<WeaponId, std::pair<std::string_view, std::wstring_view>> names;
};

class WeaponNamesStorage {
public:
    [[nodiscard]] std::string_view getWeaponName(WeaponId weaponID) const
    {
        return weaponNames.getWeaponName(weaponID);
    }

    [[nodiscard]] std::wstring_view getWeaponNameUpper(WeaponId weaponID) const
    {
        return weaponNames.getWeaponNameUpper(weaponID);
    }

    void add(WeaponId weaponID, std::string_view name, std::wstring_view nameUpperCase)
    {
        weaponNames.add(weaponID, pool.add(name), poolWide.add(nameUpperCase));
    }

private:
    StringPool<char> pool;
    StringPool<wchar_t> poolWide;
    WeaponNames weaponNames;
};

[[nodiscard]] static WeaponNamesStorage createWeaponNamesStorage(ItemSchema* itemSchema)
{
    WeaponNamesStorage storage;
    ToUtf8Converter converter{ *interfaces->localize };
    Helpers::ToUpperConverter toUpperConverter;

    for (const auto& node : itemSchema->itemsSorted) {
        const auto item = node.value;
        const auto nameWide = interfaces->localize->findSafe(item->getItemBaseName());
        storage.add(item->getWeaponId(), converter.convertUnicodeToAnsi(nameWide), toUpperConverter.toUpper(nameWide));
    }
    return storage;
}

[[nodiscard]] static const WeaponNamesStorage& getWeaponNamesInstance()
{
    static const WeaponNamesStorage storage{ createWeaponNamesStorage(memory->itemSystem()->getItemSchema()) };
    return storage;
}

class StaticDataImpl {
public:
    static const StaticDataImpl& instance() noexcept
    {
        static const StaticDataImpl staticData;
        return staticData;
    }

    static std::span<const game_items::Item> gameItems() noexcept { return instance().container.getStorage().getItems(); }
    static const auto& container_() noexcept { return instance().container; }
    static const auto& cases() noexcept { return instance()._cases; }
    static const auto& caseLoot() noexcept { return instance()._caseLoot; }

private:
    StaticDataImpl(const StaticDataImpl&) = delete;

    struct KitWeapon {
        KitWeapon(int paintKit, WeaponId weaponId, const char* iconPath) noexcept : paintKit{ paintKit }, weaponId{ weaponId }, iconPath{ iconPath } {}
        int paintKit;
        WeaponId weaponId;
        const char* iconPath;
    };

    static std::vector<KitWeapon> getKitsWeapons(const UtlMap<std::uint64_t, AlternateIconData>& alternateIcons) noexcept
    {
        std::vector<KitWeapon> kitsWeapons;
        kitsWeapons.reserve(alternateIcons.numElements);

        for (const auto& node : alternateIcons) {
            // https://github.com/perilouswithadollarsign/cstrike15_src/blob/f82112a2388b841d72cb62ca48ab1846dfcc11c8/game/shared/econ/econ_item_schema.cpp#L325-L329
            if (const auto encoded = node.key; (encoded & 3) == 0)
                kitsWeapons.emplace_back(int((encoded & 0xFFFF) >> 2), WeaponId(encoded >> 16), node.value.simpleName.data());
        }
        std::ranges::sort(kitsWeapons, {}, &KitWeapon::paintKit);
        return kitsWeapons;
    }

    void initSkinData(ItemSchema* itemSchema, game_items::Storage& storage, ToUtf8Converter<>& converter, Helpers::ToUpperConverter& toUpperConverter) noexcept
    {
        const auto kitsWeapons = getKitsWeapons(itemSchema->alternateIcons);

        for (const auto& node : itemSchema->paintKits) {
            const auto paintKit = node.value;

            if (paintKit->id == 0 || paintKit->id == 9001) // ignore workshop_default
                continue;

            const auto paintKitName = interfaces->localize->findSafe(paintKit->itemName.data());
            storage.addPaintKit(paintKit->id, game_items::ItemName{ converter.convertUnicodeToAnsi(paintKitName), toUpperConverter.toUpper(paintKitName) }, paintKit->wearRemapMin, paintKit->wearRemapMax);

            const auto isGlove = (paintKit->id >= 10000);
            for (auto it = std::ranges::lower_bound(kitsWeapons, paintKit->id, {}, &KitWeapon::paintKit); it != kitsWeapons.end() && it->paintKit == paintKit->id; ++it) {
                const auto itemDef = itemSchema->getItemDefinitionInterface(it->weaponId);
                if (!itemDef)
                    continue;

                if (isGlove) {
                    storage.addGlovesWithLastPaintKit(static_cast<EconRarity>(paintKit->rarity), it->weaponId, it->iconPath);
                } else {
                    storage.addSkinWithLastPaintKit(static_cast<EconRarity>(std::clamp(itemDef->getRarity() + paintKit->rarity - 1, 0, (paintKit->rarity == 7) ? 7 : 6)), it->weaponId, it->iconPath);
                }
            }
        }
    }

    void initStickerData(ItemSchema* itemSchema, game_items::Storage& storage, ToUtf8Converter<>& converter, Helpers::ToUpperConverter& toUpperConverter) noexcept
    {
        const auto& stickerMap = itemSchema->stickerKits;
        for (const auto& node : stickerMap) {
            const auto stickerKit = node.value;
            if (stickerKit->id == 0)
                continue;

            const auto name = std::string_view{ stickerKit->name.data(), static_cast<std::size_t>(stickerKit->name.length - 1) };
            const auto isPatch = name.starts_with("patch") || name.starts_with("stockh2021_teampatch");
            const auto isGraffiti = !isPatch && (name.starts_with("spray") || name.ends_with("graffiti"));
            const auto isSticker = !isPatch && !isGraffiti;
            
            if (isSticker) {
                const auto isGolden = name.ends_with("gold");
                const auto stickerName = interfaces->localize->findSafe(stickerKit->id != 242 ? stickerKit->itemName.data() : "StickerKit_dhw2014_teamdignitas_gold");
                storage.addSticker(stickerKit->id, game_items::ItemName{ converter.convertUnicodeToAnsi(stickerName), toUpperConverter.toUpper(stickerName) }, static_cast<EconRarity>(stickerKit->rarity),stickerKit->inventoryImage.data(), stickerKit->tournamentID, static_cast<TournamentTeam>(stickerKit->tournamentTeamID), stickerKit->tournamentPlayerID, isGolden);
            } else if (isPatch) {
                const auto patchName = interfaces->localize->findSafe(stickerKit->itemName.data());
                storage.addPatch(stickerKit->id, game_items::ItemName{ converter.convertUnicodeToAnsi(patchName), toUpperConverter.toUpper(patchName) }, static_cast<EconRarity>(stickerKit->rarity), stickerKit->inventoryImage.data());
            } else if (isGraffiti) {
                const auto paintName = interfaces->localize->findSafe(stickerKit->itemName.data());
                storage.addGraffiti(stickerKit->id, game_items::ItemName{ converter.convertUnicodeToAnsi(paintName), toUpperConverter.toUpper(paintName) }, static_cast<EconRarity>(stickerKit->rarity), stickerKit->inventoryImage.data());
            }
        }
    }

    void initMusicData(ItemSchema* itemSchema, game_items::Storage& storage, ToUtf8Converter<>& converter, Helpers::ToUpperConverter& toUpperConverter) noexcept
    {
        for (const auto& node : itemSchema->musicKits) {
            const auto musicKit = node.value;
            if (musicKit->id == 1 || musicKit->id == 2)
                continue;

            const auto musicName = interfaces->localize->findSafe(musicKit->nameLocalized);
            storage.addMusic(musicKit->id, game_items::ItemName{ converter.convertUnicodeToAnsi(musicName), toUpperConverter.toUpper(musicName) }, musicKit->inventoryImage);
        }
    }

    void initItemData(ItemSchema* itemSchema, game_items::Storage& storage, std::vector<int>& lootListIndices) noexcept
    {
        for (const auto& node : itemSchema->itemsSorted) {
            const auto item = node.value;
            const auto itemTypeName = std::string_view{ item->getItemTypeName() };
            const auto isCollectible = (itemTypeName == "#CSGO_Type_Collectible");
            const auto isOriginal = (item->getQuality() == 1);
 
            const auto inventoryImage = item->getInventoryImage();
            if (!inventoryImage)
                continue;

            const auto rarity = EconRarity{ item->getRarity() };

            if (const auto weaponID = item->getWeaponId(); itemTypeName == "#CSGO_Type_Knife" && rarity == EconRarity::Red) {
                storage.addVanillaKnife(weaponID, inventoryImage);
            } else if (isCollectible) {
                if (item->isServiceMedal()) {
                    storage.addServiceMedal(rarity, item->getServiceMedalYear(), weaponID, inventoryImage);
                } else if (item->isTournamentCoin()) {
                    storage.addTournamentCoin(rarity, weaponID, static_cast<std::uint8_t>(item->getTournamentEventID()), static_cast<std::uint16_t>(item->getStickerID()), inventoryImage);
                } else {
                    storage.addCollectible(rarity, weaponID, isOriginal, inventoryImage);
                }
            } else if (itemTypeName == "#CSGO_Tool_Name_TagTag") {
                storage.addNameTag(rarity, weaponID, inventoryImage);
            } else if (item->isPatchable()) {
                storage.addAgent(rarity, weaponID, inventoryImage);
            } else if (itemTypeName == "#CSGO_Type_WeaponCase" && item->hasCrateSeries()) {
                const auto lootListIdx = itemSchema->revolvingLootLists.find(item->getCrateSeriesNumber());
                if (lootListIdx == -1)
                    continue;

                lootListIndices.push_back(lootListIdx);
                StaticData::Case caseData;
                caseData.souvenirPackageTournamentID = item->getTournamentEventID();
                _cases.push_back(std::move(caseData));
                storage.addCase(rarity, weaponID, _cases.size() - 1, inventoryImage);
            } else if (itemTypeName == "#CSGO_Tool_WeaponCase_KeyTag") {
                storage.addCaseKey(rarity, weaponID, inventoryImage);
            } else if (const auto tool = item->getEconTool()) {
                if (std::strcmp(tool->typeName, "season_pass") == 0)
                    storage.addOperationPass(rarity, weaponID, inventoryImage);
                else if (std::strcmp(tool->typeName, "stattrak_swap") == 0)
                    storage.addStatTrakSwapTool(rarity, weaponID, inventoryImage);
                else if (std::strcmp(tool->typeName, "fantoken") == 0) {
                    if (Helpers::isSouvenirToken(weaponID))
                        storage.addSouvenirToken(rarity, weaponID, item->getTournamentEventID(), inventoryImage);
                    else
                        storage.addViewerPass(rarity, weaponID, item->getTournamentEventID(), inventoryImage);
                }
            } else if (item->isPaintable()) {
                storage.addVanillaSkin(weaponID, inventoryImage);
            }
        }
    }

    void fillLootFromLootList(ItemSchema* itemSchema, EconLootListDefinition* lootList, std::vector<std::reference_wrapper<const game_items::Item>>& loot, bool* willProduceStatTrak = nullptr) const noexcept
    {
        if (willProduceStatTrak)
            *willProduceStatTrak = *willProduceStatTrak || lootList->willProduceStatTrak();

        const auto& contents = lootList->getLootListContents();
        for (int j = 0; j < contents.size; ++j) {
            if (contents[j].stickerKit != 0) {
                if (auto idx = container.findSticker(contents[j].stickerKit); idx != std::nullopt)
                    loot.push_back(*idx);
                else if ((idx = container.findGraffiti(contents[j].stickerKit)) != std::nullopt)
                    loot.push_back(*idx);
                else if ((idx = container.findPatch(contents[j].stickerKit)) != std::nullopt)
                    loot.push_back(*idx);
            } else if (contents[j].musicKit != 0) {
                if (const auto idx = container.findMusic(contents[j].musicKit); idx.has_value())
                    loot.push_back(*idx);
            } else if (contents[j].isNestedList) {
                if (const auto nestedLootList = itemSchema->getLootList(contents[j].itemDef))
                    fillLootFromLootList(itemSchema, nestedLootList, loot, willProduceStatTrak);
            } else if (contents[j].itemDef != 0) {
                if (contents[j].paintKit != 0) {
                    if (const auto idx = container.findItem(contents[j].weaponId(), contents[j].paintKit); idx.has_value())
                        loot.push_back(*idx);
                } else {
                    if (const auto idx = container.findItem(contents[j].weaponId()); idx.has_value())
                        loot.push_back(*idx);
                }
            }
        }
    }

    // a few loot lists aren't present in client item schema, so we need to provide them ourselves
    void rebuildMissingLootList(ItemSchema* itemSchema, int lootListID, std::vector<std::reference_wrapper<const game_items::Item>>& loot) const noexcept
    {
        if (lootListID == 292) { // crate_xray_p250_lootlist
            if (const auto idx = container.findItem(WeaponId::P250, 125 /* cu_xray_p250 */); idx.has_value())
                loot.push_back(*idx);
        } else if (lootListID == 6 || lootListID == 13) { // crate_dhw13_promo and crate_ems14_promo
            constexpr auto dreamHack2013Collections = std::array{ "set_dust_2", "set_italy", "set_lake", "set_mirage", "set_safehouse", "set_train" }; // https://blog.counter-strike.net/index.php/2013/11/8199/
            for (const auto collection : dreamHack2013Collections) {
                if (const auto lootList = itemSchema->getLootList(collection)) [[likely]]
                    fillLootFromLootList(itemSchema, lootList, loot);
            }
        }
    }

    void buildLootLists(ItemSchema* itemSchema, const std::vector<int>& lootListIndices) noexcept
    {
        assert(lootListIndices.size() == _cases.size());

        for (std::size_t i = 0; i < lootListIndices.size(); ++i) {
            const auto lootListName = itemSchema->revolvingLootLists.memory[lootListIndices[i]].value;

            _cases[i].lootBeginIdx = _caseLoot.size();
            if (const auto lootList = itemSchema->getLootList(lootListName))
                fillLootFromLootList(itemSchema, lootList, _caseLoot, &_cases[i].willProduceStatTrak);
            else
                rebuildMissingLootList(itemSchema, itemSchema->revolvingLootLists.memory[lootListIndices[i]].key, _caseLoot);
            _cases[i].lootEndIdx = _caseLoot.size();

            if (_cases[i].souvenirPackageTournamentID != 0)
                _cases[i].tournamentMap = StaticData::getTournamentMapOfSouvenirPackage(lootListName);
        }
    }

    void computeRarities() noexcept
    {
        for (auto& crate : _cases) {
            for (auto it = _caseLoot.begin() + crate.lootBeginIdx; it != _caseLoot.begin() + crate.lootEndIdx; ++it)
                crate.rarities.set(it->get().getRarity());
        }
    }

    void sortLoot() noexcept
    {
        for (const auto& crate : _cases) {
            std::ranges::sort(_caseLoot.begin() + crate.lootBeginIdx, _caseLoot.begin() + crate.lootEndIdx, {}, [](const game_items::Item& item) { return item.getRarity(); });
        }
    }

    StaticDataImpl()
    {
        assert(memory && interfaces);

        const auto itemSchema = memory->itemSystem()->getItemSchema();
        game_items::Storage storage;
        ToUtf8Converter converter{ *interfaces->localize };
        Helpers::ToUpperConverter toUpperConverter;

        initSkinData(itemSchema, storage, converter, toUpperConverter);
        initStickerData(itemSchema, storage, converter, toUpperConverter);
        initMusicData(itemSchema, storage, converter, toUpperConverter);
        std::vector<int> lootListIndices;
        initItemData(itemSchema, storage, lootListIndices);
        storage.compress();
        container = game_items::Lookup{ std::move(storage) };

        buildLootLists(itemSchema, lootListIndices);
        computeRarities();
        sortLoot();

        _cases.shrink_to_fit();
        _caseLoot.shrink_to_fit();
    }

    game_items::Lookup container;
    std::vector<StaticData::Case> _cases;
    std::vector<std::reference_wrapper<const game_items::Item>> _caseLoot;
};

[[nodiscard]] std::size_t StaticData::getGameItemsCount() noexcept
{
    return StaticDataImpl::gameItems().size();
}

std::span<const std::reference_wrapper<const game_items::Item>> StaticData::getCrateLoot(const StaticData::Case& crate) noexcept
{
    if (!crate.hasLoot())
        return {};

    return { StaticDataImpl::caseLoot().begin() + crate.lootBeginIdx, StaticDataImpl::caseLoot().begin() + crate.lootEndIdx };
}

std::span<const std::reference_wrapper<const game_items::Item>> StaticData::getCrateLootOfRarity(const StaticData::Case& crate, EconRarity rarity) noexcept
{
    return ranges::equal_range(getCrateLoot(crate), rarity, {}, &game_items::Item::getRarity);
}

const StaticData::Case& StaticData::getCase(const game_items::Item& item) noexcept
{
    assert(item.isCase());
    return StaticDataImpl::cases()[item.getDataIndex()];
}

bool StaticData::isSouvenirPackage(const game_items::Item& crate) noexcept
{
    const auto& crateData = getCase(crate);
    if (crateData.souvenirPackageTournamentID == 0)
        return false;
    const auto loot = getCrateLoot(crateData);
    return !loot.empty() && loot[0].get().isSkin();
}

std::wstring_view StaticData::getWeaponNameUpper(WeaponId weaponID) noexcept
{
    return getWeaponNamesInstance().getWeaponNameUpper(weaponID);
}

std::string_view StaticData::getWeaponName(WeaponId weaponID) noexcept
{
    return getWeaponNamesInstance().getWeaponName(weaponID);
}

const game_items::Lookup& StaticData::lookup() noexcept
{
    return StaticDataImpl::container_();
}

int StaticData::getTournamentMapGoldStickerID(TournamentMap map) noexcept
{
    switch (map) {
    case TournamentMap::Ancient: return 1689;
    case TournamentMap::Dust2: return 1690;
    case TournamentMap::Inferno: return 1691;
    case TournamentMap::Mirage: return 1692;
    case TournamentMap::Nuke: return 1693;
    case TournamentMap::Overpass: return 1694;
    case TournamentMap::Vertigo: return 1695;
    default: return 0;
    }
}
