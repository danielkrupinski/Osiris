#pragma once

#include <chrono>

#include <InventoryChanger/GameItems/Storage.h>
#include <InventoryChanger/Inventory/Item.h>
#include <InventoryChanger/Inventory/Structs.h>

#include <CSGO/Constants/StickerId.h>

#include "ItemGenerator.h"

namespace inventory_changer::item_generator
{

template <typename AttributeGenerator, typename SystemClock = std::chrono::system_clock>
class DefaultGenerator {
public:
    explicit DefaultGenerator(const game_items::Storage& gameItemStorage, AttributeGenerator attributeGenerator)
        : gameItemStorage{ gameItemStorage }, attributeGenerator{ attributeGenerator } {}

    [[nodiscard]] inventory::Item::VariantProperties createVariantProperties(const game_items::Item& item) const
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

    [[nodiscard]] inventory::Item::CommonProperties createCommonProperties(const game_items::Item& item) const
    {
        if (item.isCaseKey() || (item.isCrate() && gameItemStorage.getTournamentEventID(item) == csgo::Tournament::IemRio2022))
            return { .tradableAfterDate = static_cast<std::uint32_t>(SystemClock::to_time_t(getTradableAfterWeekDate())), .purchasedFromStore = true };
        return {};
    }

private:
    [[nodiscard]] inventory::Skin createSkin(const game_items::Item& item) const
    {
        const auto& paintKit = gameItemStorage.getPaintKit(item);

        inventory::Skin skin;
        skin.wear = std::lerp(paintKit.wearRemapMin, paintKit.wearRemapMax, attributeGenerator.generateFactoryNewPaintKitWear());
        skin.seed = attributeGenerator.generatePaintKitSeed();

        if (Helpers::isMP5LabRats(item.getWeaponID(), gameItemStorage.getPaintKit(item).id))
            skin.stickers[3].stickerID = csgo::StickerId::BlacksiteFoil;

        return skin;
    }

    [[nodiscard]] inventory::Gloves createGloves(const game_items::Item& item) const
    {
        const auto& paintKit = gameItemStorage.getPaintKit(item);
        return inventory::Gloves{
            .wear = std::lerp(paintKit.wearRemapMin, paintKit.wearRemapMax, attributeGenerator.generateFactoryNewPaintKitWear()),
            .seed = attributeGenerator.generatePaintKitSeed()
        };
    }

    [[nodiscard]] inventory::ServiceMedal createServiceMedal(const game_items::Item& item) const
    {
        return inventory::ServiceMedal{
            .issueDateTimestamp = getRandomDateTimestampOfYear(gameItemStorage.getServiceMedalYear(item))
        };
    }

    [[nodiscard]] inventory::SouvenirPackage createSouvenirPackage(const game_items::Item& item) const
    {
        return attributeGenerator.generateSouvenirPackage(gameItemStorage.getTournamentEventID(item), gameItemStorage.getTournamentMap(item));
    }

    [[nodiscard]] typename SystemClock::time_point getTradableAfterWeekDate() const
    {
        using namespace std::chrono;
        constexpr auto hourWhenItemsBecomeTradable = 7h;
        constexpr auto tradablePenalty = days{ 7 } + hourWhenItemsBecomeTradable;

        return ceil<days>(SystemClock::now() - hourWhenItemsBecomeTradable) + tradablePenalty;
    }

    [[nodiscard]] static std::pair<std::time_t, std::time_t> clampTimespanToNow(std::time_t min, std::time_t max) noexcept
    {
        const auto now = SystemClock::to_time_t(SystemClock::now());
        return std::make_pair((std::min)(min, now), (std::min)(max, now));
    }

    [[nodiscard]] std::uint32_t getRandomDateTimestampOfYear(std::uint16_t year) const noexcept
    {
        const auto [min, max] = clampTimespanToNow(getStartOfYearTimestamp(year), getEndOfYearTimestamp(year));
        return attributeGenerator.generateTimestamp(static_cast<std::uint32_t>(min), static_cast<std::uint32_t>(max));
    }

    const game_items::Storage& gameItemStorage;
    AttributeGenerator attributeGenerator;
};

}
