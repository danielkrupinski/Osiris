#include <algorithm>
#include <cstring>
#include <numeric>

#include "GameItemStorage.h"
#include "StaticData.h"

#include "../Helpers.h"
#include "../Interfaces.h"
#include "../SDK/ItemSchema.h"
#include "../SDK/Localize.h"
#include "../StringPool.h"

using StaticData::TournamentMap;
using StaticData::InvalidItemIdx;
using StaticData::Type;

constexpr auto operator<=>(WeaponId a, WeaponId b) noexcept
{
    return static_cast<std::underlying_type_t<WeaponId>>(a) <=> static_cast<std::underlying_type_t<WeaponId>>(b);
}

struct ServiceMedal {
    explicit ServiceMedal(std::uint32_t year) : yearsSince2015{ static_cast<std::uint8_t>((std::max)(year, 2015u) - 2015) } {}
    std::uint16_t getServiceYear() const noexcept
    {
        return yearsSince2015 + 2015;
    }
private:
    std::uint8_t yearsSince2015;
};

struct Collectible {
    explicit Collectible(bool isOriginal) : isOriginal{ isOriginal } {}

    bool isOriginal;
};

class StaticDataImpl {
private:
    auto getTournamentStickers(std::uint32_t tournamentID) const noexcept
    {
        // not using std::ranges::equal_range() here because clang 12 on linux doesn't support it yet
        const auto begin = std::lower_bound(_tournamentStickersSorted.begin(), _tournamentStickersSorted.end(), tournamentID, [this](std::size_t index, std::uint32_t tournamentID) {
            const auto& item = _gameItems.get(index);
            assert(item.isSticker());
            return _paintKits[item.dataIndex].tournamentID < tournamentID;
        });

        const auto end = std::upper_bound(_tournamentStickersSorted.begin(), _tournamentStickersSorted.end(), tournamentID, [this](std::uint32_t tournamentID, std::size_t index) {
            const auto& item = _gameItems.get(index);
            assert(item.isSticker());
            return _paintKits[item.dataIndex].tournamentID > tournamentID;
        });

        return std::make_pair(begin, end);
    }

    auto findItems(WeaponId weaponID) const noexcept
    {
        struct Comp {
            explicit Comp(std::span<const StaticData::GameItem> gameItems) : gameItems{ gameItems } {}
            bool operator()(WeaponId weaponID, std::size_t index) const noexcept { return weaponID < gameItems[index].weaponID; }
            bool operator()(std::size_t index, WeaponId weaponID) const noexcept { return gameItems[index].weaponID < weaponID; }
        private:
            std::span<const StaticData::GameItem> gameItems;
        };

        assert(!_itemsSorted.empty());
        return std::equal_range(_itemsSorted.cbegin(), _itemsSorted.cend(), weaponID, Comp{ _gameItems.get() }); // not using std::ranges::equal_range() here because clang 12 on linux doesn't support it yet
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
        assert(_gameItems.get(*it).isSticker());
        return _paintKits[_gameItems.get(*it).dataIndex].tournamentID == tournamentID ? _paintKits[_gameItems.get(*it).dataIndex].id : 0;
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

        const auto it = std::ranges::lower_bound(range.first, range.second, team, {}, [this](std::size_t index) {
            const auto& item = _gameItems.get(index);
            assert(item.isSticker());
            return _paintKits[item.dataIndex].tournamentTeam;
        });
        if (it == range.second)
            return 0;
        assert(_gameItems.get(*it).isSticker());
        return _paintKits[_gameItems.get(*it).dataIndex].tournamentTeam == team ? _paintKits[_gameItems.get(*it).dataIndex].id : 0;
    }

    int getTournamentPlayerGoldStickerID(std::uint32_t tournamentID, int tournamentPlayerID) const noexcept
    {
        const auto range = getTournamentStickers(tournamentID);
        const auto it = std::ranges::find(range.first, range.second, tournamentPlayerID, [this](std::size_t index) { return _paintKits[_gameItems.get(index).dataIndex].tournamentPlayerID; });
        return (it != range.second ? _paintKits[_gameItems.get(*it).dataIndex].id : 0);
    }

    [[nodiscard]] StaticData::ItemIndex getItemIndex(WeaponId weaponID, int paintKit) const noexcept
    {
        const auto [begin, end] = findItems(weaponID);
        if (const auto it = std::lower_bound(begin, end, paintKit, [this](std::size_t index, int paintKit) { return _gameItems.get(index).hasPaintKit() && _paintKits[_gameItems.get(index).dataIndex].id < paintKit; }); it != end && _gameItems.get(*it).weaponID == weaponID && (!_gameItems.get(*it).hasPaintKit() || _paintKits[_gameItems.get(*it).dataIndex].id == paintKit))
            return *it;
        return InvalidItemIdx;
    }

    static const auto& gameItems() noexcept { return instance()._gameItems; }
    static const auto& collectibles() noexcept { return instance()._collectibles; }
    static const auto& cases() noexcept { return instance()._cases; }
    static const auto& caseLoot() noexcept { return instance()._caseLoot; }
    static const auto& paintKits() noexcept { return instance()._paintKits; }
    static const auto& musicKits() noexcept { return instance()._musicKits; }

    [[nodiscard]] std::wstring_view getWeaponNameUpper(WeaponId weaponID) const noexcept
    {
        if (const auto it = _weaponNamesUpper.find(weaponID); it != _weaponNamesUpper.end())
            return it->second;
        return L"";
    }

    [[nodiscard]] std::string_view getWeaponName(WeaponId weaponID) const noexcept
    {
        if (const auto it = _weaponNames.find(weaponID); it != _weaponNames.end())
            return it->second;
        return "";
    }

    [[nodiscard]] std::uint16_t getServiceMedalYear(const StaticData::GameItem& serviceMedal) const noexcept
    {
        assert(serviceMedal.isServiceMedal());
        return _serviceMedals[serviceMedal.dataIndex].getServiceYear();
    }

    [[nodiscard]] bool isCollectibleGenuine(const StaticData::GameItem& collectible) const noexcept
    {
        assert(collectible.isCollectible());
        return _collectibles[collectible.dataIndex].isOriginal;
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

        _gameItems.reserve(itemSchema->paintKits.lastAlloc);
        for (const auto& node : itemSchema->paintKits) {
            const auto paintKit = node.value;

            if (paintKit->id == 0 || paintKit->id == 9001) // ignore workshop_default
                continue;

            const auto paintKitName = interfaces->localize->findSafe(paintKit->itemName.data());
            _paintKits.emplace_back(paintKit->id, interfaces->localize->convertUnicodeToAnsi(paintKitName), Helpers::toUpper(paintKitName), paintKit->wearRemapMin, paintKit->wearRemapMax);

            const auto isGlove = (paintKit->id >= 10000);
            for (auto it = std::ranges::lower_bound(kitsWeapons, paintKit->id, {}, &KitWeapon::paintKit); it != kitsWeapons.end() && it->paintKit == paintKit->id; ++it) {
                const auto itemDef = itemSchema->getItemDefinitionInterface(it->weaponId);
                if (!itemDef)
                    continue;

                if (isGlove) {
                    _gameItems.addGloves(paintKit->rarity, it->weaponId, _paintKits.size() - 1, stringPool.add(it->iconPath));
                } else {
                    _gameItems.addSkin(std::clamp(itemDef->getRarity() + paintKit->rarity - 1, 0, (paintKit->rarity == 7) ? 7 : 6), it->weaponId, _paintKits.size() - 1, stringPool.add(it->iconPath));
                }
            }
        }
    }

    std::size_t addGraffitiPaint(int id, std::string name, std::wstring nameUpperCase)
    {
        _paintKits.emplace_back(id, std::move(name), std::move(nameUpperCase));
        return _paintKits.size() - 1;
    }

    void addPatch(int id, std::string name, std::wstring nameUpperCase, int rarity, const char* inventoryImage)
    {
        _paintKits.emplace_back(id, std::move(name), std::move(nameUpperCase));
        _gameItems.addPatch(rarity, _paintKits.size() - 1, stringPool.add(inventoryImage));
    }

    void initStickerData(ItemSchema* itemSchema) noexcept
    {
        const auto& stickerMap = itemSchema->stickerKits;
        _gameItems.reserve(_gameItems.size() + stickerMap.numElements);
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
                _paintKits.emplace_back(stickerKit->id, interfaces->localize->convertUnicodeToAnsi(stickerName), Helpers::toUpper(stickerName), stickerKit->tournamentID, static_cast<TournamentTeam>(stickerKit->tournamentTeamID), stickerKit->tournamentPlayerID, isGolden);
                _gameItems.addSticker(stickerKit->rarity, _paintKits.size() - 1, stringPool.add(stickerKit->inventoryImage.data()));
            } else if (isPatch) {
                const auto patchName = interfaces->localize->findSafe(stickerKit->itemName.data());
                addPatch(stickerKit->id, interfaces->localize->convertUnicodeToAnsi(patchName), Helpers::toUpper(patchName), stickerKit->rarity, stickerKit->inventoryImage.data());
            } else if (isGraffiti) {
                const auto paintName = interfaces->localize->findSafe(stickerKit->itemName.data());
                const auto paintID = addGraffitiPaint(stickerKit->id, interfaces->localize->convertUnicodeToAnsi(paintName), Helpers::toUpper(paintName));
                _gameItems.addGraffiti(stickerKit->rarity, paintID, stringPool.add(stickerKit->inventoryImage.data()));
                _gameItems.addSealedGraffiti(stickerKit->rarity, paintID, stringPool.add(stickerKit->inventoryImage.data()));
            }
        }
    }

    void addMusic(int musicID, std::wstring name, const char* inventoryImage)
    {
        _musicKits.emplace_back(musicID, stringPool.add(interfaces->localize->convertUnicodeToAnsi(name.c_str())), stringPoolWide.add(Helpers::toUpper(name)));
        _gameItems.addMusicKit(3, _musicKits.size() - 1, stringPool.add(inventoryImage));
    }

    void initMusicData(ItemSchema* itemSchema) noexcept
    {
        for (const auto& node : itemSchema->musicKits) {
            const auto musicKit = node.value;
            if (musicKit->id == 1 || musicKit->id == 2)
                continue;

            addMusic(musicKit->id, interfaces->localize->findSafe(musicKit->nameLocalized), musicKit->inventoryImage);
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
                _gameItems.addSkin(6, weaponID, vanillaPaintIndex, stringPool.add(inventoryImage));
            } else if (isCollectible) {
                if (item->isServiceMedal()) {
                    _serviceMedals.emplace_back(item->getServiceMedalYear());
                    _gameItems.addServiceMedal(rarity, weaponID, _serviceMedals.size() - 1, stringPool.add(inventoryImage));
                } else if (item->isTournamentCoin()) {
                    _gameItems.addTournamentCoin(rarity, weaponID, item->getTournamentEventID(), stringPool.add(inventoryImage));
                } else {
                    _collectibles.emplace_back(isOriginal);
                    _gameItems.addCollectible(rarity, weaponID, _collectibles.size() - 1, stringPool.add(inventoryImage));
                }
            } else if (itemTypeName == "#CSGO_Tool_Name_TagTag") {
                _gameItems.addNameTag(rarity, weaponID, stringPool.add(inventoryImage));
            } else if (item->isPatchable()) {
                _gameItems.addAgent(rarity, weaponID, stringPool.add(inventoryImage));
            } else if (itemTypeName == "#CSGO_Type_WeaponCase" && item->hasCrateSeries()) {
                const auto lootListIdx = itemSchema->revolvingLootLists.find(item->getCrateSeriesNumber());
                if (lootListIdx == -1)
                    continue;

                lootListIndices.push_back(lootListIdx);
                StaticData::Case caseData;
                caseData.souvenirPackageTournamentID = item->getTournamentEventID();
                _cases.push_back(std::move(caseData));
                _gameItems.addCase(rarity, weaponID, _cases.size() - 1, stringPool.add(inventoryImage));
            } else if (itemTypeName == "#CSGO_Tool_WeaponCase_KeyTag") {
                _gameItems.addCaseKey(rarity, weaponID, stringPool.add(inventoryImage));
            } else if (const auto tool = item->getEconTool()) {
                if (std::strcmp(tool->typeName, "season_pass") == 0)
                    _gameItems.addOperationPass(rarity, weaponID, stringPool.add(inventoryImage));
                else if (std::strcmp(tool->typeName, "stattrak_swap") == 0)
                    _gameItems.addStatTrakSwapTool(rarity, weaponID, stringPool.add(inventoryImage));
                else if (std::strcmp(tool->typeName, "fantoken") == 0) {
                    if (Helpers::isSouvenirToken(weaponID))
                        _gameItems.addSouvenirToken(rarity, weaponID, item->getTournamentEventID(), stringPool.add(inventoryImage));
                    else
                        _gameItems.addViewerPass(rarity, weaponID, item->getTournamentEventID(), stringPool.add(inventoryImage));
                }
            } else if (item->isPaintable()) {
                _gameItems.addSkin(0, weaponID, vanillaPaintIndex, stringPool.add(inventoryImage));
            }
        }
    }

    void initWeaponNames(ItemSchema* itemSchema) noexcept
    {
        for (const auto weaponID : _gameItems.getUniqueWeaponIDs()) {
            const auto def = itemSchema->getItemDefinitionInterface(weaponID);
            if (!def)
                continue;

            std::wstring nameWide = interfaces->localize->findSafe(def->getItemBaseName());
            /*
            if (item.isCollectible() && _collectibles[item.dataIndex].isOriginal) {
                nameWide += L" (";
                nameWide += interfaces->localize->findSafe("genuine");
                nameWide += L")";
            }
            */
            _weaponNames.emplace(weaponID, stringPool.add(interfaces->localize->convertUnicodeToAnsi(nameWide.c_str())));
            _weaponNamesUpper.emplace(weaponID, stringPoolWide.add(Helpers::toUpper(std::move(nameWide))));
        }
    }

    void fillLootFromLootList(ItemSchema* itemSchema, EconLootListDefinition* lootList, std::vector<std::size_t>& loot, bool* willProduceStatTrak = nullptr) const noexcept
    {
        if (willProduceStatTrak)
            *willProduceStatTrak = *willProduceStatTrak || lootList->willProduceStatTrak();

        const auto& contents = lootList->getLootListContents();
        for (int j = 0; j < contents.size; ++j) {
            if (contents[j].stickerKit != 0) {
                if (auto idx = getItemIndex(WeaponId::Sticker, contents[j].stickerKit); idx != InvalidItemIdx)
                    loot.push_back(idx);
                else if ((idx = getItemIndex(WeaponId::Graffiti, contents[j].stickerKit)) != InvalidItemIdx)
                    loot.push_back(idx);
                else if ((idx = getItemIndex(WeaponId::Patch, contents[j].stickerKit)) != InvalidItemIdx)
                    loot.push_back(idx);
            } else if (contents[j].musicKit != 0) {
                if (const auto idx = getItemIndex(WeaponId::MusicKit, contents[j].musicKit); idx != InvalidItemIdx)
                    loot.push_back(idx);
            } else if (contents[j].isNestedList) {
                if (const auto nestedLootList = itemSchema->getLootList(contents[j].itemDef))
                    fillLootFromLootList(itemSchema, nestedLootList, loot, willProduceStatTrak);
            } else if (contents[j].itemDef != 0) {
                if (const auto idx = getItemIndex(contents[j].weaponId(), contents[j].paintKit); idx != InvalidItemIdx)
                    loot.push_back(idx);
            }
        }
    }

    // a few loot lists aren't present in client item schema, so we need to provide them ourselves
    void rebuildMissingLootList(ItemSchema* itemSchema, int lootListID, std::vector<std::size_t>& loot) const noexcept
    {
        if (lootListID == 292) { // crate_xray_p250_lootlist
            if (const auto idx = getItemIndex(WeaponId::P250, 125 /* cu_xray_p250 */); idx != InvalidItemIdx)
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

    void initSortedItemsVector() noexcept
    {
        _itemsSorted.resize(_gameItems.size());
        std::iota(_itemsSorted.begin(), _itemsSorted.end(), 0);

        std::ranges::sort(_itemsSorted, [this](std::size_t a, std::size_t b) {
            const auto& itemA = _gameItems.get(a);
            const auto& itemB = _gameItems.get(b);
            if (itemA.weaponID == itemB.weaponID && itemA.hasPaintKit() && itemB.hasPaintKit())
                return _paintKits[itemA.dataIndex].id < _paintKits[itemB.dataIndex].id;
            return itemA.weaponID < itemB.weaponID;
        });
    }

    void initTournamentSortedStickers() noexcept
    {
        assert(!_itemsSorted.empty());

        const auto stickers = findItems(WeaponId::Sticker);
        _tournamentStickersSorted = { stickers.first, stickers.second };

        std::ranges::sort(_tournamentStickersSorted, [this](std::size_t a, std::size_t b) {
            const auto& itemA = _gameItems.get(a);
            const auto& itemB = _gameItems.get(b);
            assert(itemA.isSticker() && itemB.isSticker());

            const auto& paintKitA = _paintKits[itemA.dataIndex];
            const auto& paintKitB = _paintKits[itemB.dataIndex];
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
        return std::all_of(_caseLoot.begin() + caseData.lootBeginIdx, _caseLoot.begin() + caseData.lootEndIdx, [this](std::size_t itemIndex) { return _gameItems.get(itemIndex).isSticker(); });
    }

    [[nodiscard]] bool isPatchPack(const StaticData::Case& caseData) const noexcept
    {
        return std::all_of(_caseLoot.begin() + caseData.lootBeginIdx, _caseLoot.begin() + caseData.lootEndIdx, [this](std::size_t itemIndex) { return _gameItems.get(itemIndex).isPatch(); });
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
        initWeaponNames(itemSchema);

        _gameItems.sort([this](const auto& a, const auto& b) {
            if (a.weaponID == b.weaponID && a.hasPaintKit() && b.hasPaintKit())
                return _paintKits[a.dataIndex].nameUpperCase < _paintKits[b.dataIndex].nameUpperCase;

            const auto comp = _weaponNamesUpper[a.weaponID].compare(_weaponNamesUpper[b.weaponID]);
            if (comp == 0)
                return a.weaponID < b.weaponID;
            return comp < 0;
        });

        initSortedItemsVector();
        buildLootLists(itemSchema, lootListIndices);
        excludeTournamentStickerCapsulesFromSouvenirPackages();
        initTournamentSortedStickers();

        _gameItems.shrinkToFit();
        _collectibles.shrink_to_fit();
        _cases.shrink_to_fit();
        _caseLoot.shrink_to_fit();
        _itemsSorted.shrink_to_fit();
        _tournamentStickersSorted.shrink_to_fit();
        _paintKits.shrink_to_fit();
    }

    StringPool<char> stringPool;
    StringPool<wchar_t> stringPoolWide;
    GameItemStorage _gameItems;
    std::vector<Collectible> _collectibles;
    std::vector<ServiceMedal> _serviceMedals;
    std::vector<StaticData::Case> _cases;
    std::vector<StaticData::ItemIndex> _caseLoot;
    std::vector<StaticData::ItemIndex> _itemsSorted;
    std::vector<StaticData::ItemIndex> _tournamentStickersSorted;
    std::vector<StaticData::PaintKit> _paintKits{ { 0, "", L"" } };
    static constexpr auto vanillaPaintIndex = 0;
    std::unordered_map<WeaponId, std::string_view> _weaponNames;
    std::unordered_map<WeaponId, std::wstring_view> _weaponNamesUpper;
    std::vector<StaticData::MusicKit> _musicKits;
};

std::span<const StaticData::GameItem> StaticData::gameItems() noexcept
{
    return StaticDataImpl::gameItems().get();
}

const std::vector<StaticData::ItemIndex>& StaticData::caseLoot() noexcept
{
    return StaticDataImpl::caseLoot();
}

[[nodiscard]] int StaticData::getStickerID(const GameItem& item) noexcept
{
    assert(item.isSticker());
    return StaticDataImpl::paintKits()[item.dataIndex].id;
}

[[nodiscard]] int StaticData::getMusicID(const GameItem& item) noexcept
{
    assert(item.isMusic());
    return StaticDataImpl::musicKits()[item.dataIndex].id;
}

[[nodiscard]] int StaticData::getPatchID(const GameItem& item) noexcept
{
    assert(item.isPatch());
    return StaticDataImpl::paintKits()[item.dataIndex].id;
}

[[nodiscard]] int StaticData::getSkinPaintID(const GameItem& item) noexcept
{
    assert(item.isSkin());
    return StaticDataImpl::paintKits()[item.dataIndex].id;
}

std::string_view StaticData::getPaintName(const GameItem& item) noexcept
{
    if (item.hasPaintKit())
        return StaticDataImpl::paintKits()[item.dataIndex].name;
    if (item.isMusic())
        return StaticDataImpl::musicKits()[item.dataIndex].name;
    return "";
}

const StaticData::PaintKit& StaticData::getPaintKit(const GameItem& item) noexcept
{
    assert(item.hasPaintKit());
    return StaticDataImpl::paintKits()[item.dataIndex];
}

const StaticData::Case& StaticData::getCase(const GameItem& item) noexcept
{
    assert(item.isCase());
    return StaticDataImpl::cases()[item.dataIndex];
}

const StaticData::GameItem& StaticData::getGameItem(ItemIndex itemIndex) noexcept
{
    return StaticDataImpl::gameItems().get(itemIndex);
}

std::wstring_view StaticData::getWeaponNameUpper(WeaponId weaponID) noexcept
{
    return StaticDataImpl::instance().getWeaponNameUpper(weaponID);
}

std::string_view StaticData::getWeaponName(WeaponId weaponID) noexcept
{
    return StaticDataImpl::instance().getWeaponName(weaponID);
}

StaticData::ItemIndex StaticData::getItemIndex(WeaponId weaponID, int paintKit) noexcept
{
    return StaticDataImpl::instance().getItemIndex(weaponID, paintKit);
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

StaticData::GameItem::GameItem(Type type, int rarity, WeaponId weaponID, std::size_t dataIndex, std::string_view iconPath) noexcept : type{ type }, rarity{ static_cast<std::uint8_t>(rarity) }, weaponID{ weaponID }, dataIndex{ dataIndex }, iconPath{ iconPath } {}

StaticData::PaintKit::PaintKit(int id, std::string name, std::wstring nameUpperCase) noexcept : id{ id }, name{ std::move(name) }, nameUpperCase{ std::move(nameUpperCase) } {}

StaticData::PaintKit::PaintKit(int id, std::string name, std::wstring nameUpperCase, float wearRemapMin, float wearRemapMax) noexcept : id{ id }, wearRemapMin{ wearRemapMin }, wearRemapMax{ wearRemapMax }, name{ std::move(name) }, nameUpperCase{ std::move(nameUpperCase) } {}

StaticData::PaintKit::PaintKit(int id, std::string name, std::wstring nameUpperCase, std::uint32_t tournamentID, TournamentTeam tournamentTeam, int tournamentPlayerID, bool isGoldenSticker) noexcept : id{ id }, name{ std::move(name) }, nameUpperCase{ std::move(nameUpperCase) }, tournamentID{ tournamentID }, tournamentTeam{ tournamentTeam }, tournamentPlayerID{ tournamentPlayerID }, isGoldenSticker{ isGoldenSticker } {}
