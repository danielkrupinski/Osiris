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

std::wstring_view StaticData::getWeaponNameUpper(WeaponId weaponID) noexcept
{
    return getWeaponNamesInstance().getWeaponNameUpper(weaponID);
}

std::string_view StaticData::getWeaponName(WeaponId weaponID) noexcept
{
    return getWeaponNamesInstance().getWeaponName(weaponID);
}
