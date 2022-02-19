#include <algorithm>
#include <cstring>
#include <numeric>
#include <string_view>
#include <unordered_map>
#include <utility>

#include "GameItemStorage.h"
#include "StaticData.h"

#include "../Helpers.h"
#include "../Interfaces.h"
#include "../SDK/ItemSchema.h"
#include "../SDK/Localize.h"
#include "../StringPool.h"
#include "StaticDataStorage.h"

using StaticData::TournamentMap;
using StaticData::Type;

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

    for (const auto& node : itemSchema->itemsSorted) {
        const auto item = node.value;
        const auto nameWide = interfaces->localize->findSafe(item->getItemBaseName());
        storage.add(item->getWeaponId(), converter.convertUnicodeToAnsi(nameWide), Helpers::toUpper(nameWide));
    }
    return storage;
}

[[nodiscard]] static const WeaponNamesStorage& getWeaponNamesInstance()
{
    static const WeaponNamesStorage storage{ createWeaponNamesStorage(memory->itemSystem()->getItemSchema()) };
    return storage;
}

class StaticDataImpl {
private:
    auto getTournamentStickers(std::uint32_t tournamentID) const noexcept
    {
        // not using std::ranges::equal_range() here because clang 12 on linux doesn't support it yet
        const auto begin = std::lower_bound(_tournamentStickersSorted.begin(), _tournamentStickersSorted.end(), tournamentID, [this](const StaticData::GameItem& item, std::uint32_t tournamentID) {
            assert(item.isSticker());
            return storage.getStickerKit(item).tournamentID < tournamentID;
        });

        const auto end = std::upper_bound(_tournamentStickersSorted.begin(), _tournamentStickersSorted.end(), tournamentID, [this](std::uint32_t tournamentID, const StaticData::GameItem& item) {
            assert(item.isSticker());
            return storage.getStickerKit(item).tournamentID > tournamentID;
        });

        return std::make_pair(begin, end);
    }

    auto findItems(WeaponId weaponID) const noexcept
    {
        struct Comp {
            bool operator()(WeaponId weaponID, const StaticData::GameItem& item) const noexcept { return weaponID < item.weaponID; }
            bool operator()(const StaticData::GameItem& item, WeaponId weaponID) const noexcept { return item.weaponID < weaponID; }
        };

        return std::equal_range(storage.getGameItems().cbegin(), storage.getGameItems().cend(), weaponID, Comp{}); // not using std::ranges::equal_range() here because clang 12 on linux doesn't support it yet
    }

public:
    static const StaticDataImpl& instance() noexcept
    {
        static const StaticDataImpl staticData;
        return staticData;
    }

    int getTournamentEventStickerID(std::uint32_t tournamentID) const noexcept
    {
        if (tournamentID == 1) // DreamHack 2013
            return Helpers::random(1, 12);
        else if (tournamentID == 3) // EMS One Katowice 2014
            return Helpers::random(99, 100); // EMS Wolf / Skull
        else if (tournamentID == 4) // ELS One Cologne 2014
            return 172;

        const auto it = getTournamentStickers(tournamentID).first;
        if (it == _tournamentStickersSorted.end())
            return 0;
        assert(it->get().isSticker());
        return storage.getStickerKit(*it).tournamentID == tournamentID ? storage.getStickerKit(*it).id : 0;
    }

    int getTournamentTeamGoldStickerID(std::uint32_t tournamentID, TournamentTeam team) const noexcept
    {
        if (tournamentID == 0 || team == TournamentTeam::None)
            return 0;

        if (team == TournamentTeam::AllStarTeamAmerica)
            return 1315;
        if (team == TournamentTeam::AllStarTeamEurope)
            return 1316;

        const auto range = getTournamentStickers(tournamentID);

        const auto it = std::ranges::lower_bound(range.first, range.second, team, {}, [this](const StaticData::GameItem& item) {
            assert(item.isSticker());
            return storage.getStickerKit(item).tournamentTeam;
        });
        if (it == range.second)
            return 0;
        assert(it->get().isSticker());
        return storage.getStickerKit(*it).tournamentTeam == team ? storage.getStickerKit(*it).id : 0;
    }

    int getTournamentPlayerGoldStickerID(std::uint32_t tournamentID, int tournamentPlayerID) const noexcept
    {
        const auto range = getTournamentStickers(tournamentID);
        const auto it = std::ranges::find(range.first, range.second, tournamentPlayerID, [this](const StaticData::GameItem& item) { return storage.getStickerKit(item).tournamentPlayerID; });
        return (it != range.second ? storage.getStickerKit(*it).id : 0);
    }

    [[nodiscard]] StaticData::ItemIndex2 getItemIndex(WeaponId weaponID, int paintKit) const noexcept
    {
        const auto [begin, end] = findItems(weaponID);
        if (const auto it = std::lower_bound(begin, end, paintKit, [this](const StaticData::GameItem& item, int paintKit) { return item.hasPaintKit() && storage.getPaintKit(item).id < paintKit; }); it != end && it->weaponID == weaponID && (!it->hasPaintKit() || storage.getPaintKit(*it).id == paintKit))
            return StaticData::ItemIndex2{ static_cast<std::size_t>(std::distance(storage.getGameItems().begin(), it)) };
        return StaticData::InvalidItemIdx2;
    }

    [[nodiscard]] StaticData::ItemIndex2 getItemIndex(WeaponId weaponID) const noexcept
    {
        if (const auto it = std::ranges::lower_bound(storage.getGameItems(), weaponID, {}, &StaticData::GameItem::weaponID); it != storage.getGameItems().end())
            return StaticData::ItemIndex2{ static_cast<std::size_t>(std::distance(storage.getGameItems().begin(), it)) };
        return StaticData::InvalidItemIdx2;
    }

    [[nodiscard]] StaticData::ItemIndex2 getMusicIndex(int musicKit) const noexcept
    {
        const auto [begin, end] = findItems(WeaponId::MusicKit);
        if (const auto it = std::find_if(begin, end, [this, musicKit](const StaticData::GameItem& item) { return storage.getMusicKit(item).id == musicKit; }); it != end)
            return StaticData::ItemIndex2{ static_cast<std::size_t>(std::distance(storage.getGameItems().begin(), it)) };
        return StaticData::InvalidItemIdx2;
    }

    [[nodiscard]] std::optional<std::reference_wrapper<const StaticData::GameItem>> getMusic(int musicKit) const noexcept
    {
        const auto [begin, end] = findItems(WeaponId::MusicKit);
        if (const auto it = std::find_if(begin, end, [this, musicKit](const StaticData::GameItem& item) { return storage.getMusicKit(item).id == musicKit; }); it != end)
            return *it;
        return {};
    }

    [[nodiscard]] StaticData::ItemIndex2 getStickerIndex(int stickerKit) const noexcept
    {
        const auto [begin, end] = findItems(WeaponId::Sticker);
        if (const auto it = std::find_if(begin, end, [this, stickerKit](const StaticData::GameItem& item) { return storage.getStickerKit(item).id == stickerKit; }); it != end)
            return StaticData::ItemIndex2{ static_cast<std::size_t>(std::distance(storage.getGameItems().begin(), it)) };
        return StaticData::InvalidItemIdx2;
    }

    [[nodiscard]] std::optional<std::reference_wrapper<const StaticData::GameItem>> getSticker(int stickerKit) const noexcept
    {
        const auto [begin, end] = findItems(WeaponId::Sticker);
        if (const auto it = std::find_if(begin, end, [this, stickerKit](const StaticData::GameItem& item) { return storage.getStickerKit(item).id == stickerKit; }); it != end)
            return *it;
        return {};
    }

    static const auto& gameItems() noexcept { return instance().storage.getGameItems(); }
    static const auto& cases() noexcept { return instance()._cases; }
    static const auto& caseLoot() noexcept { return instance()._caseLoot; }
    static const auto& getStickerKit(const StaticData::GameItem& item) noexcept { return instance().storage.getStickerKit(item); }
    static const auto& getPaintKit(const StaticData::GameItem& item) noexcept { return instance().storage.getPaintKit(item); }
    static const auto& getMusicKit(const StaticData::GameItem& item) noexcept { return instance().storage.getMusicKit(item); }

    [[nodiscard]] std::uint16_t getServiceMedalYear(const StaticData::GameItem& serviceMedal) const noexcept
    {
        return storage.getServiceMedalYear(serviceMedal);
    }

    [[nodiscard]] bool isCollectibleGenuine(const StaticData::GameItem& collectible) const noexcept
    {
        return storage.isCollectibleGenuine(collectible);
    }

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

    void initSkinData(ItemSchema* itemSchema) noexcept
    {
        const auto kitsWeapons = getKitsWeapons(itemSchema->alternateIcons);

        ToUtf8Converter converter{ *interfaces->localize };

        storage.getGameItems().reserve(itemSchema->paintKits.lastAlloc);
        for (const auto& node : itemSchema->paintKits) {
            const auto paintKit = node.value;

            if (paintKit->id == 0 || paintKit->id == 9001) // ignore workshop_default
                continue;

            const auto paintKitName = interfaces->localize->findSafe(paintKit->itemName.data());
            storage.addPaintKit(paintKit->id, StaticData::ItemName{ stringPool.add(converter.convertUnicodeToAnsi(paintKitName)), stringPoolWide.add(Helpers::toUpper(paintKitName)) }, paintKit->wearRemapMin, paintKit->wearRemapMax);

            const auto isGlove = (paintKit->id >= 10000);
            for (auto it = std::ranges::lower_bound(kitsWeapons, paintKit->id, {}, &KitWeapon::paintKit); it != kitsWeapons.end() && it->paintKit == paintKit->id; ++it) {
                const auto itemDef = itemSchema->getItemDefinitionInterface(it->weaponId);
                if (!itemDef)
                    continue;

                if (isGlove) {
                    storage.addGlovesWithLastPaintKit(paintKit->rarity, it->weaponId, stringPool.add(it->iconPath));
                } else {
                    storage.addSkinWithLastPaintKit(std::clamp(itemDef->getRarity() + paintKit->rarity - 1, 0, (paintKit->rarity == 7) ? 7 : 6), it->weaponId, stringPool.add(it->iconPath));
                }
            }
        }
    }

    void initStickerData(ItemSchema* itemSchema) noexcept
    {
        ToUtf8Converter converter{ *interfaces->localize };

        const auto& stickerMap = itemSchema->stickerKits;
        storage.getGameItems().reserve(storage.getGameItems().size() + stickerMap.numElements);
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
                storage.addSticker(stickerKit->id, StaticData::ItemName{ stringPool.add(converter.convertUnicodeToAnsi(stickerName)), stringPoolWide.add(Helpers::toUpper(stickerName)) }, stickerKit->rarity, stringPool.add(stickerKit->inventoryImage.data()), stickerKit->tournamentID, static_cast<TournamentTeam>(stickerKit->tournamentTeamID), stickerKit->tournamentPlayerID, isGolden);
            } else if (isPatch) {
                const auto patchName = interfaces->localize->findSafe(stickerKit->itemName.data());
                storage.addPatch(stickerKit->id, StaticData::ItemName{ stringPool.add(converter.convertUnicodeToAnsi(patchName)), stringPoolWide.add(Helpers::toUpper(patchName)) }, stickerKit->rarity, stringPool.add(stickerKit->inventoryImage.data()));
            } else if (isGraffiti) {
                const auto paintName = interfaces->localize->findSafe(stickerKit->itemName.data());
                storage.addGraffiti(stickerKit->id, StaticData::ItemName{ stringPool.add(converter.convertUnicodeToAnsi(paintName)), stringPoolWide.add(Helpers::toUpper(paintName)) }, stickerKit->rarity, stringPool.add(stickerKit->inventoryImage.data()));
            }
        }
    }

    void initMusicData(ItemSchema* itemSchema) noexcept
    {
        ToUtf8Converter converter{ *interfaces->localize };

        for (const auto& node : itemSchema->musicKits) {
            const auto musicKit = node.value;
            if (musicKit->id == 1 || musicKit->id == 2)
                continue;

            const auto musicName = interfaces->localize->findSafe(musicKit->nameLocalized);
            storage.addMusic(musicKit->id, StaticData::ItemName{ stringPool.add(converter.convertUnicodeToAnsi(musicName)), stringPoolWide.add(Helpers::toUpper(musicName)) }, stringPool.add(musicKit->inventoryImage));
        }
    }

    void initItemData(ItemSchema* itemSchema, std::vector<int>& lootListIndices) noexcept
    {
        for (const auto& node : itemSchema->itemsSorted) {
            const auto item = node.value;
            const auto itemTypeName = std::string_view{ item->getItemTypeName() };
            const auto isCollectible = (itemTypeName == "#CSGO_Type_Collectible");
            const auto isOriginal = (item->getQuality() == 1);
 
            const auto inventoryImage = item->getInventoryImage();
            if (!inventoryImage)
                continue;

            const auto rarity = item->getRarity();

            if (const auto weaponID = item->getWeaponId(); itemTypeName == "#CSGO_Type_Knife" && rarity == 6) {
                storage.addVanillaKnife(weaponID, stringPool.add(inventoryImage));
            } else if (isCollectible) {
                if (item->isServiceMedal()) {
                    storage.addServiceMedal(rarity, item->getServiceMedalYear(), weaponID, stringPool.add(inventoryImage));
                } else if (item->isTournamentCoin()) {
                    storage.addTournamentCoin(rarity, weaponID, item->getTournamentEventID(), stringPool.add(inventoryImage));
                } else {
                    storage.addCollectible(rarity, weaponID, isOriginal, stringPool.add(inventoryImage));
                }
            } else if (itemTypeName == "#CSGO_Tool_Name_TagTag") {
                storage.addNameTag(rarity, weaponID, stringPool.add(inventoryImage));
            } else if (item->isPatchable()) {
                storage.addAgent(rarity, weaponID, stringPool.add(inventoryImage));
            } else if (itemTypeName == "#CSGO_Type_WeaponCase" && item->hasCrateSeries()) {
                const auto lootListIdx = itemSchema->revolvingLootLists.find(item->getCrateSeriesNumber());
                if (lootListIdx == -1)
                    continue;

                lootListIndices.push_back(lootListIdx);
                StaticData::Case caseData;
                caseData.souvenirPackageTournamentID = item->getTournamentEventID();
                _cases.push_back(std::move(caseData));
                storage.addCase(rarity, weaponID, _cases.size() - 1, stringPool.add(inventoryImage));
            } else if (itemTypeName == "#CSGO_Tool_WeaponCase_KeyTag") {
                storage.addCaseKey(rarity, weaponID, stringPool.add(inventoryImage));
            } else if (const auto tool = item->getEconTool()) {
                if (std::strcmp(tool->typeName, "season_pass") == 0)
                    storage.addOperationPass(rarity, weaponID, stringPool.add(inventoryImage));
                else if (std::strcmp(tool->typeName, "stattrak_swap") == 0)
                    storage.addStatTrakSwapTool(rarity, weaponID, stringPool.add(inventoryImage));
                else if (std::strcmp(tool->typeName, "fantoken") == 0) {
                    if (Helpers::isSouvenirToken(weaponID))
                        storage.addSouvenirToken(rarity, weaponID, item->getTournamentEventID(), stringPool.add(inventoryImage));
                    else
                        storage.addViewerPass(rarity, weaponID, item->getTournamentEventID(), stringPool.add(inventoryImage));
                }
            } else if (item->isPaintable()) {
                storage.addVanillaSkin(weaponID, stringPool.add(inventoryImage));
            }
        }
    }

    void fillLootFromLootList(ItemSchema* itemSchema, EconLootListDefinition* lootList, std::vector<StaticData::ItemIndex2>& loot, bool* willProduceStatTrak = nullptr) const noexcept
    {
        if (willProduceStatTrak)
            *willProduceStatTrak = *willProduceStatTrak || lootList->willProduceStatTrak();

        const auto& contents = lootList->getLootListContents();
        for (int j = 0; j < contents.size; ++j) {
            if (contents[j].stickerKit != 0) {
                if (auto idx = getStickerIndex(contents[j].stickerKit); idx != StaticData::InvalidItemIdx2)
                    loot.push_back(idx);
                else if ((idx = getItemIndex(WeaponId::SealedGraffiti, contents[j].stickerKit)) != StaticData::InvalidItemIdx2)
                    loot.push_back(idx);
                else if ((idx = getItemIndex(WeaponId::Patch, contents[j].stickerKit)) != StaticData::InvalidItemIdx2)
                    loot.push_back(idx);
            } else if (contents[j].musicKit != 0) {
                if (const auto idx = getMusicIndex(contents[j].musicKit); idx != StaticData::InvalidItemIdx2)
                    loot.push_back(idx);
            } else if (contents[j].isNestedList) {
                if (const auto nestedLootList = itemSchema->getLootList(contents[j].itemDef))
                    fillLootFromLootList(itemSchema, nestedLootList, loot, willProduceStatTrak);
            } else if (contents[j].itemDef != 0) {
                if (const auto idx = getItemIndex(contents[j].weaponId(), contents[j].paintKit); idx != StaticData::InvalidItemIdx2)
                    loot.push_back(idx);
            }
        }
    }

    // a few loot lists aren't present in client item schema, so we need to provide them ourselves
    void rebuildMissingLootList(ItemSchema* itemSchema, int lootListID, std::vector<StaticData::ItemIndex2>& loot) const noexcept
    {
        if (lootListID == 292) { // crate_xray_p250_lootlist
            if (const auto idx = getItemIndex(WeaponId::P250, 125 /* cu_xray_p250 */); idx != StaticData::InvalidItemIdx2)
                loot.push_back(idx);
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

            if (_cases[i].isSouvenirPackage())
                _cases[i].tournamentMap = StaticData::getTournamentMapOfSouvenirPackage(lootListName);
        }
    }

    void initTournamentSortedStickers() noexcept
    {
        const auto stickers = findItems(WeaponId::Sticker);
        _tournamentStickersSorted = { stickers.first, stickers.second };

        std::ranges::sort(_tournamentStickersSorted, [this](const StaticData::GameItem& itemA, const StaticData::GameItem& itemB) {
            assert(itemA.isSticker() && itemB.isSticker());

            const auto& paintKitA = storage.getStickerKit(itemA);
            const auto& paintKitB = storage.getStickerKit(itemB);
            if (paintKitA.tournamentID != paintKitB.tournamentID)
                return paintKitA.tournamentID < paintKitB.tournamentID;
            if (paintKitA.tournamentTeam != paintKitB.tournamentTeam)
                return paintKitA.tournamentTeam < paintKitB.tournamentTeam;
            if (paintKitA.tournamentPlayerID != paintKitB.tournamentPlayerID)
                return paintKitA.tournamentPlayerID < paintKitB.tournamentPlayerID;
            if (paintKitA.isGoldenSticker != paintKitB.isGoldenSticker)
                return paintKitA.isGoldenSticker;
            return itemA.rarity > itemB.rarity;
        });
    }

    [[nodiscard]] bool isStickerCapsule(const StaticData::Case& caseData) const noexcept
    {
        return std::all_of(_caseLoot.begin() + caseData.lootBeginIdx, _caseLoot.begin() + caseData.lootEndIdx, [this](StaticData::ItemIndex2 itemIndex) { return storage.getGameItems().get(itemIndex.value).isSticker(); });
    }

    [[nodiscard]] bool isPatchPack(const StaticData::Case& caseData) const noexcept
    {
        return std::all_of(_caseLoot.begin() + caseData.lootBeginIdx, _caseLoot.begin() + caseData.lootEndIdx, [this](StaticData::ItemIndex2 itemIndex) { return storage.getGameItems().get(itemIndex.value).isPatch(); });
    }

    void excludeTournamentStickerCapsulesFromSouvenirPackages() noexcept
    {
        for (auto& crate : _cases) {
            if (isStickerCapsule(crate) || isPatchPack(crate))
                crate.souvenirPackageTournamentID = 0;
        }
    }

    StaticDataImpl()
    {
        assert(memory && interfaces);

        const auto itemSchema = memory->itemSystem()->getItemSchema();
        initSkinData(itemSchema);
        initStickerData(itemSchema);
        initMusicData(itemSchema);
        std::vector<int> lootListIndices;
        initItemData(itemSchema, lootListIndices);

        std::ranges::sort(storage.getGameItems(), [this](const StaticData::GameItem& itemA, const StaticData::GameItem& itemB) {
            if (itemA.weaponID == itemB.weaponID && itemA.hasPaintKit() && itemB.hasPaintKit())
                return storage.getPaintKit(itemA).id < storage.getPaintKit(itemB).id;
            return itemA.weaponID < itemB.weaponID;
        });

        buildLootLists(itemSchema, lootListIndices);
        excludeTournamentStickerCapsulesFromSouvenirPackages();
        initTournamentSortedStickers();

        _cases.shrink_to_fit();
        _caseLoot.shrink_to_fit();
        _tournamentStickersSorted.shrink_to_fit();
    }

    StringPool<char> stringPool;
    StringPool<wchar_t> stringPoolWide;
    StaticDataStorage storage;
    std::vector<StaticData::Case> _cases;
    std::vector<StaticData::ItemIndex2> _caseLoot;
    std::vector<std::reference_wrapper<const StaticData::GameItem>> _tournamentStickersSorted;
};

[[nodiscard]] std::size_t StaticData::getGameItemsCount() noexcept
{
    return StaticDataImpl::gameItems().size();
}

const std::vector<StaticData::ItemIndex2>& StaticData::caseLoot() noexcept
{
    return StaticDataImpl::caseLoot();
}

std::vector<StaticData::ItemIndex2> StaticData::getItemIndices() noexcept
{
    const auto itemIndexCount = StaticDataImpl::gameItems().size();
    std::vector<StaticData::ItemIndex2> indices(itemIndexCount);
    for (std::size_t i = 0; i < itemIndexCount; ++i)
        indices[i] = StaticData::ItemIndex2{ i };
    return indices;
}

[[nodiscard]] int StaticData::getStickerID(const GameItem& item) noexcept
{
    assert(item.isSticker());
    return StaticDataImpl::getStickerKit(item).id;
}

[[nodiscard]] int StaticData::getMusicID(const GameItem& item) noexcept
{
    assert(item.isMusic());
    return StaticDataImpl::getMusicKit(item).id;
}

[[nodiscard]] int StaticData::getPatchID(const GameItem& item) noexcept
{
    assert(item.isPatch());
    return StaticDataImpl::getPaintKit(item).id;
}

[[nodiscard]] int StaticData::getSkinPaintID(const GameItem& item) noexcept
{
    assert(item.isSkin());
    return StaticDataImpl::getPaintKit(item).id;
}

int StaticData::getGraffitiID(const GameItem& item) noexcept
{
    assert(item.isGraffiti());
    return StaticDataImpl::getPaintKit(item).id;
}

int StaticData::getSealedGraffitiID(const GameItem& item) noexcept
{
    assert(item.isSealedGraffiti());
    return StaticDataImpl::getPaintKit(item).id;
}

std::string_view StaticData::getPaintName(const GameItem& item) noexcept
{
    if (item.hasPaintKit())
        return StaticDataImpl::getPaintKit(item).name.forDisplay;
    if (item.isMusic())
        return StaticDataImpl::getMusicKit(item).name.forDisplay;
    if (item.isSticker())
        return StaticDataImpl::getStickerKit(item).name.forDisplay;
    return "";
}

std::wstring_view StaticData::getPaintNameUpper(const GameItem& item) noexcept
{
    if (item.hasPaintKit())
        return StaticDataImpl::getPaintKit(item).name.forSearch;
    if (item.isMusic())
        return StaticDataImpl::getMusicKit(item).name.forSearch;
    if (item.isSticker())
        return StaticDataImpl::getStickerKit(item).name.forSearch;
    return L"";
}

const StaticData::PaintKit& StaticData::getPaintKit(const GameItem& item) noexcept
{
    assert(item.hasPaintKit());
    return StaticDataImpl::getPaintKit(item);
}

const StaticData::Case& StaticData::getCase(const GameItem& item) noexcept
{
    assert(item.isCase());
    return StaticDataImpl::cases()[item.dataIndex];
}

const StaticData::GameItem& StaticData::getGameItem(ItemIndex2 itemIndex) noexcept
{
    return StaticDataImpl::gameItems().get(itemIndex.value);
}

std::wstring_view StaticData::getWeaponNameUpper(WeaponId weaponID) noexcept
{
    return getWeaponNamesInstance().getWeaponNameUpper(weaponID);
}

std::string_view StaticData::getWeaponName(WeaponId weaponID) noexcept
{
    return getWeaponNamesInstance().getWeaponName(weaponID);
}

StaticData::ItemIndex2 StaticData::getItemIndex(WeaponId weaponID, int paintKit) noexcept
{
    return StaticDataImpl::instance().getItemIndex(weaponID, paintKit);
}

StaticData::ItemIndex2 StaticData::getItemIndex(WeaponId weaponID) noexcept
{
    return StaticDataImpl::instance().getItemIndex(weaponID);
}

StaticData::ItemIndex2 StaticData::getMusicIndex(int musicID) noexcept
{
    return StaticDataImpl::instance().getMusicIndex(musicID);
}

int StaticData::findSouvenirTournamentSticker(std::uint32_t tournamentID) noexcept
{
    return StaticDataImpl::instance().getTournamentEventStickerID(tournamentID);
}

int StaticData::getTournamentTeamGoldStickerID(std::uint32_t tournamentID, TournamentTeam team) noexcept
{
    return StaticDataImpl::instance().getTournamentTeamGoldStickerID(tournamentID, team);
}

int StaticData::getTournamentPlayerGoldStickerID(std::uint32_t tournamentID, int tournamentPlayerID) noexcept
{
    return StaticDataImpl::instance().getTournamentPlayerGoldStickerID(tournamentID, tournamentPlayerID);
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

bool StaticData::isCollectibleGenuine(const GameItem& collectible) noexcept
{
    return StaticDataImpl::instance().isCollectibleGenuine(collectible);
}

std::uint16_t StaticData::getServiceMedalYear(const GameItem& serviceMedal) noexcept
{
    return StaticDataImpl::instance().getServiceMedalYear(serviceMedal);
}

std::uint32_t StaticData::getTournamentEventID(const GameItem& item) noexcept
{
    assert(item.isSouvenirToken() || item.isViewerPass() || item.isTournamentCoin());
    return static_cast<std::uint32_t>(item.dataIndex);
}

StaticData::GameItem::GameItem(Type type, int rarity, WeaponId weaponID, std::size_t dataIndex, std::string_view iconPath) noexcept : type{ type }, rarity{ static_cast<std::uint8_t>(rarity) }, weaponID{ weaponID }, dataIndex{ dataIndex }, iconPath{ iconPath } {}

StaticData::PaintKit::PaintKit(int id, ItemName name) noexcept : id{ id }, name{ name } {}

StaticData::PaintKit::PaintKit(int id, ItemName name, float wearRemapMin, float wearRemapMax) noexcept : id{ id }, wearRemapMin{ wearRemapMin }, wearRemapMax{ wearRemapMax }, name{ name } {}
