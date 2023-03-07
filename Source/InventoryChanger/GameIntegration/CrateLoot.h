#pragma once

#include <InventoryChanger/GameItems/Lookup.h>

namespace csgo
{

struct EconLootListDefinition;
struct ItemSchema;

}

namespace inventory_changer::game_items
{
    class CrateLoot;
}

namespace inventory_changer::game_integration
{

class CrateLoot {
public:
    CrateLoot(csgo::ItemSchema& itemSchema, const game_items::Lookup& gameItemLookup)
        : itemSchema{ itemSchema }, gameItemLookup{ gameItemLookup } {}

    void getLoot(game_items::CrateLoot& crateLoot);

private:
    [[nodiscard]] const game_items::Item* findStickerlikeItem(WeaponId weaponID, int stickerKit) const;
    void fillLootFromLootList(const csgo::EconLootListDefinition& lootList, game_items::CrateLoot& crateLoot);
    void rebuildMissingLootList(game_items::CrateLoot& crateLoot);

    csgo::ItemSchema& itemSchema;
    const game_items::Lookup& gameItemLookup;
};

}
