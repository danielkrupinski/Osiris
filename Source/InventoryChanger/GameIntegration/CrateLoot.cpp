#include "CrateLoot.h"

#include <InventoryChanger/GameItems/CrateLoot.h>
#include <InventoryChanger/GameItems/Lookup.h>

namespace inventory_changer::game_integration
{

void CrateLoot::getLoot(game_items::CrateLoot& crateLoot)
{
    rebuildMissingLootList(crateLoot);

    for (const auto& revolvingLootList : itemSchema.revolvingLootLists) {
        const auto lootListName = revolvingLootList.value;

        if (const auto lootList = itemSchema.getLootList(lootListName)) {
            crateLoot.nextLootList(revolvingLootList.key);
            fillLootFromLootList(*lootList, crateLoot);
        }
    }
}

game_items::Lookup::OptionalItemReference CrateLoot::findStickerlikeItem(WeaponId weaponID, int stickerKit) const
{
    switch (weaponID) {
    case WeaponId::Sticker: return gameItemLookup.findSticker(stickerKit);
    case WeaponId::Patch: return gameItemLookup.findPatch(stickerKit);
    case WeaponId::SealedGraffiti: return gameItemLookup.findGraffiti(stickerKit);
    default:
        return {};
    }
}

void CrateLoot::fillLootFromLootList(EconLootListDefinition& lootList, game_items::CrateLoot& crateLoot)
{
    if (lootList.willProduceStatTrak())
        crateLoot.setWillProduceStatTrak();

    const auto& contents = lootList.getLootListContents();
    for (int j = 0; j < contents.size; ++j) {
        if (contents[j].stickerKit != 0) {
            if (const auto item = findStickerlikeItem(contents[j].weaponId(), contents[j].stickerKit); item.has_value())
                crateLoot.addItem(*item);
        } else if (contents[j].musicKit != 0) {
            if (const auto idx = gameItemLookup.findMusic(contents[j].musicKit); idx.has_value())
                crateLoot.addItem(*idx);
        } else if (contents[j].isNestedList) {
            if (const auto nestedLootList = itemSchema.getLootList(contents[j].itemDef))
                fillLootFromLootList(*nestedLootList, crateLoot);
        } else if (contents[j].itemDef != 0) {
            if (contents[j].paintKit != 0) {
                if (const auto idx = gameItemLookup.findItem(contents[j].weaponId(), contents[j].paintKit); idx.has_value())
                    crateLoot.addItem(*idx);
            } else {
                if (const auto idx = gameItemLookup.findItem(contents[j].weaponId()); idx.has_value())
                    crateLoot.addItem(*idx);
            }
        }
    }
}

// a few loot lists aren't present in client item schema, so we need to provide them ourselves
void CrateLoot::rebuildMissingLootList(game_items::CrateLoot& crateLoot)
{
    crateLoot.nextLootList(6); // crate_dhw13_promo

    static constexpr auto dreamHack2013Collections = std::array{ "set_dust_2", "set_italy", "set_lake", "set_mirage", "set_safehouse", "set_train" }; // https://blog.counter-strike.net/index.php/2013/11/8199/
    for (const auto collection : dreamHack2013Collections) {
        if (const auto lootList = itemSchema.getLootList(collection)) [[likely]]
            fillLootFromLootList(*lootList, crateLoot);
    }

    crateLoot.nextLootListFromPrevious(13); // crate_ems14_promo

    crateLoot.nextLootList(292); // crate_xray_p250_lootlist
    if (const auto p250XRay = gameItemLookup.findItem(WeaponId::P250, 125 /* cu_xray_p250 */); p250XRay.has_value())
        crateLoot.addItem(*p250XRay);
}

}
