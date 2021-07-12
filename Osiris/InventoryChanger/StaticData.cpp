#include <algorithm>
#include <cstring>
#include <numeric>

#include "StaticData.h"

#include "../Helpers.h"
#include "../Interfaces.h"
#include "../SDK/ItemSchema.h"
#include "../SDK/Localize.h"

using namespace StaticData;

class StaticDataImpl {
public:
    static const auto& gameItems() noexcept { return instance()._gameItems; }
    static const auto& collectibles() noexcept { return instance()._collectibles; }
    static const auto& cases() noexcept { return instance()._cases; }
    static const auto& caseLoot() noexcept { return instance()._caseLoot; }
    static const auto& paintKits() noexcept { return instance()._paintKits; }
    static const auto& getWeaponNameUpper(WeaponId weaponID) noexcept { return instance()._weaponNamesUpper[weaponID]; }
    static const auto& getWeaponName(WeaponId weaponID) noexcept { return instance()._weaponNames[weaponID]; }
    static auto getItemIndex_(WeaponId weaponID, int paintKit) noexcept { return instance().getItemIndex(weaponID, paintKit); }
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

            _paintKits.emplace_back(paintKit->id, interfaces->localize->findSafe(paintKit->itemName.data()), paintKit->wearRemapMin, paintKit->wearRemapMax);

            const auto isGlove = (paintKit->id >= 10000);
            for (auto it = std::ranges::lower_bound(kitsWeapons, paintKit->id, {}, &KitWeapon::paintKit); it != kitsWeapons.end() && it->paintKit == paintKit->id; ++it) {
                const auto itemDef = itemSchema->getItemDefinitionInterface(it->weaponId);
                if (!itemDef)
                    continue;

                if (isGlove) {
                    _gameItems.emplace_back(Type::Glove, paintKit->rarity, it->weaponId, _paintKits.size() - 1, it->iconPath);
                } else {
                    _gameItems.emplace_back(Type::Skin, std::clamp(itemDef->getRarity() + paintKit->rarity - 1, 0, (paintKit->rarity == 7) ? 7 : 6), it->weaponId, _paintKits.size() - 1, it->iconPath);
                }
            }
        }
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
            const auto isPatch = name.starts_with("patch");
            const auto isGraffiti = !isPatch && (name.starts_with("spray") || name.ends_with("graffiti"));
            const auto isSticker = !isPatch && !isGraffiti;

            if (isSticker) {
                _paintKits.emplace_back(stickerKit->id, interfaces->localize->findSafe(stickerKit->id != 242 ? stickerKit->itemName.data() : "StickerKit_dhw2014_teamdignitas_gold"));
                _gameItems.emplace_back(Type::Sticker, stickerKit->rarity, WeaponId::Sticker, _paintKits.size() - 1, stickerKit->inventoryImage.data());
            } else if (isPatch) {
                _paintKits.emplace_back(stickerKit->id, interfaces->localize->findSafe(stickerKit->itemName.data()));
                _gameItems.emplace_back(Type::Patch, stickerKit->rarity, WeaponId::Patch, _paintKits.size() - 1, stickerKit->inventoryImage.data());
            } else if (isGraffiti) {
                _paintKits.emplace_back(stickerKit->id, interfaces->localize->findSafe(stickerKit->itemName.data()));
                _gameItems.emplace_back(Type::Graffiti, stickerKit->rarity, WeaponId::Graffiti, _paintKits.size() - 1, stickerKit->inventoryImage.data());
                _gameItems.emplace_back(Type::SealedGraffiti, stickerKit->rarity, WeaponId::SealedGraffiti, _paintKits.size() - 1, stickerKit->inventoryImage.data());
            }
        }
    }

    void initMusicData(ItemSchema* itemSchema) noexcept
    {
        const auto& musicMap = itemSchema->musicKits;
        for (const auto& node : musicMap) {
            const auto musicKit = node.value;
            if (musicKit->id == 1 || musicKit->id == 2)
                continue;

            std::wstring name = interfaces->localize->findSafe(musicKit->nameLocalized);
            _paintKits.emplace_back(musicKit->id, std::move(name));
            _gameItems.emplace_back(Type::Music, 3, WeaponId::MusicKit, _paintKits.size() - 1, musicKit->inventoryImage);
        }
    }

    void initItemData(ItemSchema* itemSchema, std::vector<int>& lootListIndices) noexcept
    {
        for (const auto& node : itemSchema->itemsSorted) {
            const auto item = node.value;
            const auto itemTypeName = std::string_view{ item->getItemTypeName() };
            const auto isCollectible = (itemTypeName == "#CSGO_Type_Collectible");
            const auto isOriginal = (item->getQuality() == 1);

            if (!_weaponNames.contains(item->getWeaponId())) {
                std::wstring nameWide = interfaces->localize->findSafe(item->getItemBaseName());
                if (isCollectible && isOriginal) {
                    nameWide += L" (";
                    nameWide += interfaces->localize->findSafe("genuine");
                    nameWide += L") ";
                }
                _weaponNames.emplace(item->getWeaponId(), interfaces->localize->convertUnicodeToAnsi(nameWide.c_str()));
                _weaponNamesUpper.emplace(item->getWeaponId(), Helpers::toUpper(std::move(nameWide)));
            }

            const auto inventoryImage = item->getInventoryImage();
            if (!inventoryImage)
                continue;

            if (itemTypeName == "#CSGO_Type_Knife" && item->getRarity() == 6) {
                _gameItems.emplace_back(Type::Skin, 6, item->getWeaponId(), vanillaPaintIndex, inventoryImage);
            } else if (isCollectible) {
                _collectibles.emplace_back(isOriginal);
                _gameItems.emplace_back(Type::Collectible, item->getRarity(), item->getWeaponId(), _collectibles.size() - 1, inventoryImage);
            } else if (itemTypeName == "#CSGO_Tool_Name_TagTag") {
                _gameItems.emplace_back(Type::NameTag, item->getRarity(), item->getWeaponId(), 0, inventoryImage);
            } else if (item->isPatchable()) {
                _gameItems.emplace_back(Type::Agent, item->getRarity(), item->getWeaponId(), 0, inventoryImage);
            } else if (itemTypeName == "#CSGO_Type_WeaponCase" && item->hasCrateSeries()) {
                const auto lootListIdx = itemSchema->revolvingLootLists.find(item->getCrateSeriesNumber());
                if (lootListIdx == -1)
                    continue;

                lootListIndices.push_back(lootListIdx);
                Case caseData;
                caseData.isSouvenirPackage = item->hasTournamentEventID();
                _cases.push_back(std::move(caseData));
                _gameItems.emplace_back(Type::Case, item->getRarity(), item->getWeaponId(), _cases.size() - 1, inventoryImage);
            } else if (itemTypeName == "#CSGO_Tool_WeaponCase_KeyTag") {
                _gameItems.emplace_back(Type::CaseKey, item->getRarity(), item->getWeaponId(), 0, inventoryImage);
            } else if (const auto tool = item->getEconTool()) {
                if (std::strcmp(tool->typeName, "season_pass") == 0)
                    _gameItems.emplace_back(Type::OperationPass, item->getRarity(), item->getWeaponId(), 0, inventoryImage);
                else if (std::strcmp(tool->typeName, "stattrak_swap") == 0)
                    _gameItems.emplace_back(Type::StatTrakSwapTool, item->getRarity(), item->getWeaponId(), 0, inventoryImage);
                else if (std::strcmp(tool->typeName, "fantoken") == 0)
                    _gameItems.emplace_back(Type::ViewerPass, item->getRarity(), item->getWeaponId(), 0, inventoryImage);
            }
        }
    }

    auto findItems(WeaponId weaponID) const noexcept
    {
        struct Comp {
            explicit Comp(const std::vector<GameItem>& gameItems) : gameItems{ gameItems } {}
            bool operator()(WeaponId weaponID, std::size_t index) const noexcept { return weaponID < gameItems[index].weaponID; }
            bool operator()(std::size_t index, WeaponId weaponID) const noexcept { return gameItems[index].weaponID < weaponID; }
        private:
            const std::vector<GameItem>& gameItems;
        };

        assert(!_itemsSorted.empty());
        return std::equal_range(_itemsSorted.cbegin(), _itemsSorted.cend(), weaponID, Comp{ _gameItems }); // not using std::ranges::equal_range() here because it requires extra operators
    }

    std::size_t getItemIndex(WeaponId weaponID, int paintKit) noexcept
    {
        const auto [begin, end] = findItems(weaponID);
        if (const auto it = std::lower_bound(begin, end, paintKit, [this](std::size_t index, int paintKit) { return _gameItems[index].hasPaintKit() && _paintKits[_gameItems[index].dataIndex].id < paintKit; }); it != end && _gameItems[*it].weaponID == weaponID && (!_gameItems[*it].hasPaintKit() || _paintKits[_gameItems[*it].dataIndex].id == paintKit))
            return *it;
        return InvalidItemIdx;
    }

    void fillLootFromLootList(ItemSchema* itemSchema, EconLootListDefinition* lootList, std::vector<std::size_t>& loot, bool* willProduceStatTrak = nullptr) noexcept
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
    void rebuildMissingLootList(ItemSchema* itemSchema, int lootListID, std::vector<std::size_t>& loot) noexcept
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
            _cases[i].lootBeginIdx = _caseLoot.size();
            if (const auto lootList = itemSchema->getLootList(itemSchema->revolvingLootLists.memory[lootListIndices[i]].value))
                fillLootFromLootList(itemSchema, lootList, _caseLoot, &_cases[i].willProduceStatTrak);
            else
                rebuildMissingLootList(itemSchema, itemSchema->revolvingLootLists.memory[lootListIndices[i]].key, _caseLoot);
            _cases[i].lootEndIdx = _caseLoot.size();
        }
    }

    void initSortedVectors() noexcept
    {
        _itemsSorted.resize(_gameItems.size());
        std::iota(_itemsSorted.begin(), _itemsSorted.end(), 0);

        std::ranges::sort(_itemsSorted, [this](std::size_t a, std::size_t b) {
            const auto& itemA = _gameItems[a];
            const auto& itemB = _gameItems[b];
            if (itemA.weaponID == itemB.weaponID && itemA.hasPaintKit() && itemB.hasPaintKit())
                return _paintKits[itemA.dataIndex].id < _paintKits[itemB.dataIndex].id;
            return itemA.weaponID < itemB.weaponID;
        });
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

        std::ranges::sort(_gameItems, [this](const auto& a, const auto& b) {
            if (a.weaponID == b.weaponID && a.hasPaintKit() && b.hasPaintKit())
                return _paintKits[a.dataIndex].nameUpperCase < _paintKits[b.dataIndex].nameUpperCase;
            return _weaponNamesUpper[a.weaponID] < _weaponNamesUpper[b.weaponID];
        });

        initSortedVectors();
        buildLootLists(itemSchema, lootListIndices);

        _gameItems.shrink_to_fit();
        _collectibles.shrink_to_fit();
        _cases.shrink_to_fit();
        _caseLoot.shrink_to_fit();
        _itemsSorted.shrink_to_fit();
        _paintKits.shrink_to_fit();
    }

    static StaticDataImpl& instance() noexcept
    {
        static StaticDataImpl staticData;
        return staticData;
    }

    std::vector<GameItem> _gameItems;
    std::vector<Collectible> _collectibles;
    std::vector<Case> _cases;
    std::vector<std::size_t> _caseLoot;
    std::vector<std::size_t> _itemsSorted;
    std::vector<StaticData::PaintKit> _paintKits{ { 0, L"" } };
    static constexpr auto vanillaPaintIndex = 0;
    std::unordered_map<WeaponId, std::string> _weaponNames;
    std::unordered_map<WeaponId, std::wstring> _weaponNamesUpper;
};

const std::vector<GameItem>& StaticData::gameItems() noexcept
{
    return StaticDataImpl::gameItems();
}

const std::vector<Collectible>& StaticData::collectibles() noexcept
{
    return StaticDataImpl::collectibles();
}

const std::vector<Case>& StaticData::cases() noexcept
{
    return StaticDataImpl::cases();
}

const std::vector<std::size_t>& StaticData::caseLoot() noexcept
{
    return StaticDataImpl::caseLoot();
}

const std::vector<StaticData::PaintKit>& StaticData::paintKits() noexcept
{
    return StaticDataImpl::paintKits();
}

const std::wstring& StaticData::getWeaponNameUpper(WeaponId weaponID) noexcept
{
    return StaticDataImpl::getWeaponNameUpper(weaponID);
}

const std::string& StaticData::getWeaponName(WeaponId weaponID) noexcept
{
    return StaticDataImpl::getWeaponName(weaponID);
}

std::size_t StaticData::getItemIndex(WeaponId weaponID, int paintKit) noexcept
{
    return StaticDataImpl::getItemIndex_(weaponID, paintKit);
}

StaticData::GameItem::GameItem(Type type, int rarity, WeaponId weaponID, std::size_t dataIndex, std::string&& iconPath) noexcept : type{ type }, rarity{ static_cast<std::uint8_t>(rarity) }, weaponID{ weaponID }, dataIndex{ dataIndex }, iconPath{ std::move(iconPath) } {}

StaticData::PaintKit::PaintKit(int id, std::wstring&& name, float wearRemapMin, float wearRemapMax) noexcept : id{ id }, wearRemapMin{ wearRemapMin }, wearRemapMax{ wearRemapMax }, nameUpperCase{ std::move(name) }
{
    this->name = interfaces->localize->convertUnicodeToAnsi(nameUpperCase.c_str());
    nameUpperCase = Helpers::toUpper(nameUpperCase);
}
