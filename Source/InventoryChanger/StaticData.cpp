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
#include "GameIntegration/Items.h"
#include "GameIntegration/Misc.h"

#include "GameItems/CrateLoot.h"
#include "GameItems/CrateLootLookup.h"

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
        names.emplace(weaponID, std::make_pair(pool.add(name), poolWide.add(nameUpperCase)));
    }

private:
    StringPool<char> pool;
    StringPool<wchar_t> poolWide;
    std::unordered_map<WeaponId, std::pair<std::string_view, std::wstring_view>> names;
};

[[nodiscard]] static WeaponNames createWeaponNamesStorage(ItemSchema* itemSchema)
{
    WeaponNames storage;
    ToUtf8Converter converter{ *interfaces->localize };
    Helpers::ToUpperConverter toUpperConverter;

    for (const auto& node : itemSchema->itemsSorted) {
        const auto item = node.value;
        const auto nameWide = interfaces->localize->findSafe(item->getItemBaseName());
        storage.add(item->getWeaponId(), converter.convertUnicodeToAnsi(nameWide), toUpperConverter.toUpper(nameWide));
    }
    return storage;
}

[[nodiscard]] static const WeaponNames& getWeaponNamesInstance()
{
    static const WeaponNames storage{ createWeaponNamesStorage(memory->itemSystem()->getItemSchema()) };
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
    static const auto& crateLoot() noexcept { return instance().crateLootLookup; }

private:
    StaticDataImpl(const StaticDataImpl&) = delete;

    [[nodiscard]] game_items::Lookup::OptionalItemReference findStickerlikeItem(WeaponId weaponID, int stickerKit) const
    {
        switch (weaponID) {
        case WeaponId::Sticker: return container.findSticker(stickerKit);
        case WeaponId::Patch: return container.findPatch(stickerKit);
        case WeaponId::SealedGraffiti: return container.findGraffiti(stickerKit);
        default:
            return {};
        }
    }

    void fillLootFromLootList(ItemSchema* itemSchema, EconLootListDefinition* lootList, game_items::CrateLoot& crateLoot) const noexcept
    {
        if (lootList->willProduceStatTrak())
            crateLoot.setWillProduceStatTrak();

        const auto& contents = lootList->getLootListContents();
        for (int j = 0; j < contents.size; ++j) {
            if (contents[j].stickerKit != 0) {
                if (const auto item = findStickerlikeItem(contents[j].weaponId(), contents[j].stickerKit); item.has_value())
                    crateLoot.addItem(*item);
            } else if (contents[j].musicKit != 0) {
                if (const auto idx = container.findMusic(contents[j].musicKit); idx.has_value())
                    crateLoot.addItem(*idx);
            } else if (contents[j].isNestedList) {
                if (const auto nestedLootList = itemSchema->getLootList(contents[j].itemDef))
                    fillLootFromLootList(itemSchema, nestedLootList, crateLoot);
            } else if (contents[j].itemDef != 0) {
                if (contents[j].paintKit != 0) {
                    if (const auto idx = container.findItem(contents[j].weaponId(), contents[j].paintKit); idx.has_value())
                        crateLoot.addItem(*idx);
                } else {
                    if (const auto idx = container.findItem(contents[j].weaponId()); idx.has_value())
                        crateLoot.addItem(*idx);
                }
            }
        }
    }

    // a few loot lists aren't present in client item schema, so we need to provide them ourselves
    void rebuildMissingLootList(ItemSchema* itemSchema, int lootListID, game_items::CrateLoot& crateLoot) const noexcept
    {
        if (lootListID == 292) { // crate_xray_p250_lootlist
            if (const auto p250XRay = container.findItem(WeaponId::P250, 125 /* cu_xray_p250 */); p250XRay.has_value())
                crateLoot.addItem(*p250XRay);
        }
    }

    void buildLootLists(ItemSchema* itemSchema, game_items::CrateLoot& crateLoot) noexcept
    {
        crateLoot.nextLootList(6);

        static constexpr auto dreamHack2013Collections = std::array{ "set_dust_2", "set_italy", "set_lake", "set_mirage", "set_safehouse", "set_train" }; // https://blog.counter-strike.net/index.php/2013/11/8199/
        for (const auto collection : dreamHack2013Collections) {
            if (const auto lootList = itemSchema->getLootList(collection)) [[likely]]
                fillLootFromLootList(itemSchema, lootList, crateLoot);
        }

        crateLoot.nextLootListFromPrevious(13);

        for (const auto& revolvingLootList : itemSchema->revolvingLootLists) {
            if (revolvingLootList.key == 6 || revolvingLootList.key == 13)
                continue;

            const auto lootListName = revolvingLootList.value;

            crateLoot.nextLootList(revolvingLootList.key);

            if (const auto lootList = itemSchema->getLootList(lootListName))
                fillLootFromLootList(itemSchema, lootList, crateLoot);
            else
                rebuildMissingLootList(itemSchema, revolvingLootList.key, crateLoot);
        }
    }

    StaticDataImpl()
    {
        assert(memory && interfaces);

        const auto itemSchema = memory->itemSystem()->getItemSchema();
        game_items::Storage storage;

        inventory_changer::game_integration::Items items{ *itemSchema, *interfaces->localize };
        items.getStickers(storage);
        items.getMusicKits(storage);
        items.getSkinsAndGloves(storage);
        items.getOtherItems(storage);
        storage.compress();
        container = game_items::Lookup{ std::move(storage) };

        game_items::CrateLoot crateLoot;
        buildLootLists(itemSchema, crateLoot);
        crateLoot.compress();
        crateLootLookup = game_items::CrateLootLookup{ std::move(crateLoot) };
    }

    game_items::Lookup container;
    game_items::CrateLootLookup crateLootLookup;
};

bool StaticData::isSouvenirPackage(const game_items::Item& crate) noexcept
{
    if (lookup().getStorage().getTournamentEventID(crate) == 0)
        return false;
    const auto loot = crateLoot().getLoot(lookup().getStorage().getCrateSeries(crate));
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

const game_items::CrateLootLookup& StaticData::crateLoot() noexcept
{
    return StaticDataImpl::crateLoot();
}
