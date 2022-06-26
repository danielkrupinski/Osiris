#include "WeaponNames.h"

#include <SDK/ItemSchema.h>
#include <SDK/Localize.h>

#include <Helpers.h>
#include <Interfaces.h>

namespace inventory_changer
{

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

const WeaponNames& WeaponNames::instance()
{
    static const WeaponNames weaponNames{ createWeaponNamesStorage(memory->itemSystem()->getItemSchema()) };
    return weaponNames;
}

}
