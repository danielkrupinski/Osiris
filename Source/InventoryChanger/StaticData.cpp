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
#include "GameIntegration/CrateLoot.h"
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

    static std::span<const inventory_changer::game_items::Item> gameItems() noexcept { return instance().container.getStorage().getItems(); }
    static const auto& container_() noexcept { return instance().container; }
    static const auto& crateLoot() noexcept { return instance().crateLootLookup; }

private:
    StaticDataImpl(const StaticDataImpl&) = delete;

    StaticDataImpl()
    {
        assert(memory && interfaces);

        const auto itemSchema = memory->itemSystem()->getItemSchema();
        inventory_changer::game_integration::Items items{ *itemSchema, *interfaces->localize };
        auto storage = inventory_changer::game_integration::createGameItemStorage(items);
        storage.compress();
        container = inventory_changer::game_items::Lookup{ std::move(storage) };

        inventory_changer::game_integration::CrateLoot gameLoot{ *itemSchema, container };
        inventory_changer::game_items::CrateLoot crateLoot;
        gameLoot.getLoot(crateLoot);
        crateLoot.compress();
        crateLootLookup = inventory_changer::game_items::CrateLootLookup{ std::move(crateLoot) };
    }

    inventory_changer::game_items::Lookup container;
    inventory_changer::game_items::CrateLootLookup crateLootLookup;
};

std::wstring_view StaticData::getWeaponNameUpper(WeaponId weaponID) noexcept
{
    return getWeaponNamesInstance().getWeaponNameUpper(weaponID);
}

std::string_view StaticData::getWeaponName(WeaponId weaponID) noexcept
{
    return getWeaponNamesInstance().getWeaponName(weaponID);
}
