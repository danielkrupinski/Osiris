#pragma once

#include <string_view>
#include <unordered_map>
#include <utility>

#include <StringPool.h>

#include "../Memory.h"

enum class WeaponId : short;

namespace inventory_changer
{

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

    static const WeaponNames& instance(const OtherInterfaces& interfaces, const Memory& memory);

private:
    StringPool<char> pool;
    StringPool<wchar_t> poolWide;
    std::unordered_map<WeaponId, std::pair<std::string_view, std::wstring_view>> names;
};

}
