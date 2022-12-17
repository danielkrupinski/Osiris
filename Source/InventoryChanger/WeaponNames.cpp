#include "WeaponNames.h"

#include <CSGO/ItemSchema.h>
#include <CSGO/Localize.h>
#include <CSGO/PODs/ItemSchema.h>

#include <Helpers.h>
#include <Interfaces/OtherInterfaces.h>

namespace inventory_changer
{

[[nodiscard]] static WeaponNames createWeaponNamesStorage(const OtherInterfaces& interfaces, const csgo::ItemSchemaPOD& itemSchema)
{
    WeaponNames storage;
    ToUtf8Converter converter{ interfaces.getLocalize() };
    Helpers::ToUpperConverter toUpperConverter;

    for (const auto& node : itemSchema.itemsSorted) {
        const auto item = csgo::EconItemDefinition::from(retSpoofGadgets->client, node.value);
        const auto nameWide = interfaces.getLocalize().findSafe(item.getItemBaseName());
        storage.add(item.getWeaponId(), converter.convertUnicodeToAnsi(nameWide), toUpperConverter.toUpper(nameWide));
    }
    return storage;
}

const WeaponNames& WeaponNames::instance(const OtherInterfaces& interfaces, const Memory& memory)
{
    static const WeaponNames weaponNames{ createWeaponNamesStorage(interfaces, *memory.itemSystem().getItemSchema()) };
    return weaponNames;
}

}
