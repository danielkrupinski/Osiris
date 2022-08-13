#pragma once

#include <InventoryChanger/GameItems/Storage.h>
#include <InventoryChanger/Inventory/Item.h>
#include <InventoryChanger/Inventory/Structs.h>

#include "Utils.h"

namespace inventory_changer::item_generator
{

template <typename AttributeGenerator>
class DefaultGenerator {
public:
    explicit DefaultGenerator(const game_items::Storage& gameItemStorage, AttributeGenerator attributeGenerator)
        : gameItemStorage{ gameItemStorage }, attributeGenerator{ attributeGenerator } {}

    [[nodiscard]] inventory::Item::VariantProperties createItemData(const game_items::Item& item) const
    {
        if (item.isSkin()) {
            return createSkin(item);
        } else if (item.isGloves()) {
            return createGloves(item);
        } else if (item.isCrate()) {
            if (gameItemStorage.isSouvenirPackage(item))
                return createSouvenirPackage(item);
        } else if (item.isServiceMedal()) {
            return createServiceMedal(item);
        }

        return {};
    }

private:
    [[nodiscard]] inventory::Skin createSkin(const game_items::Item& item) const
    {
        const auto& paintKit = gameItemStorage.getPaintKit(item);

        inventory::Skin skin;
        skin.wear = std::lerp(paintKit.wearRemapMin, paintKit.wearRemapMax, attributeGenerator.generatePaintKitWear(FactoryNew));
        skin.seed = attributeGenerator.generatePaintKitSeed();

        if (Helpers::isMP5LabRats(item.getWeaponID(), gameItemStorage.getPaintKit(item).id))
            skin.stickers[3].stickerID = 28;

        return skin;
    }

    [[nodiscard]] inventory::Gloves createGloves(const game_items::Item& item) const
    {
        const auto& paintKit = gameItemStorage.getPaintKit(item);
        return inventory::Gloves{
            .wear = std::lerp(paintKit.wearRemapMin, paintKit.wearRemapMax, attributeGenerator.generatePaintKitWear(FactoryNew)),
            .seed = attributeGenerator.generatePaintKitSeed()
        };
    }

    [[nodiscard]] inventory::ServiceMedal createServiceMedal(const game_items::Item& item) const
    {
        return inventory::ServiceMedal{
            .issueDateTimestamp = attributeGenerator.generateServiceMedalIssueDate(gameItemStorage.getServiceMedalYear(item))
        };
    }

    [[nodiscard]] inventory::SouvenirPackage createSouvenirPackage(const game_items::Item& item) const
    {
        return attributeGenerator.generateSouvenirPackage(gameItemStorage.getTournamentEventID(item), gameItemStorage.getTournamentMap(item));
    }

    const game_items::Storage& gameItemStorage;
    AttributeGenerator attributeGenerator;
};

}
