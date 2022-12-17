#pragma once

#include <algorithm>

#include <CSGO/WeaponId.h>

#include <InventoryChanger/GameIntegration/Misc.h>
#include <InventoryChanger/GameItems/Lookup.h>
#include <InventoryChanger/Inventory/Item.h>
#include <InventoryChanger/Inventory/Structs.h>

#include <CSGO/Constants/Tournament.h>
#include <CSGO/ItemSchema.h>

namespace inventory_changer::item_generator
{

template <typename AttributeGenerator, typename StickerSlotCountGetter>
class DropGenerator {
public:
    explicit DropGenerator(const game_items::Lookup& gameItemLookup, AttributeGenerator attributeGenerator, StickerSlotCountGetter stickerSlotCountGetter)
        : gameItemLookup{ gameItemLookup }, attributeGenerator{ attributeGenerator }, stickerSlotCountGetter{ stickerSlotCountGetter } {}

    [[nodiscard]] inventory::Item::VariantProperties createVariantProperties(const game_items::Item& unlockedItem, const inventory::Item& caseItem, bool willProduceStatTrak) const
    {
        if (willProduceStatTrak && unlockedItem.isMusic()) {
            return inventory::Music{ .statTrak = 0 };
        } else if (unlockedItem.isSkin()) {
            return generateSkin(unlockedItem, caseItem);
        } else if (unlockedItem.isGloves()) {
            return generateGloves(unlockedItem);
        }
        return {};
    }

    [[nodiscard]] inventory::Item::CommonProperties createCommonProperties(const inventory::Item& crate, const inventory::Item* crateKey) const
    {
        inventory::Item::CommonProperties properties{ .tradableAfterDate = crate.getProperties().common.tradableAfterDate };
        if (crateKey)
            properties.tradableAfterDate = (std::max)(properties.tradableAfterDate, crateKey->getProperties().common.tradableAfterDate);
        return properties;
    }

private:
    [[nodiscard]] inventory::Skin generateSkin(const game_items::Item& unlockedItem, const inventory::Item& caseItem) const
    {
        inventory::Skin skin;
        const auto& paintKit = gameItemLookup.getStorage().getPaintKit(unlockedItem);
        skin.wear = std::lerp(paintKit.wearRemapMin, paintKit.wearRemapMax, attributeGenerator.generatePaintKitWear());
        skin.seed = attributeGenerator.generatePaintKitSeed();

        if (const auto souvenirPackage = get<inventory::SouvenirPackage>(caseItem)) {
            skin.tournamentID = gameItemLookup.getStorage().getTournamentEventID(caseItem.gameItem());
            skin.tournamentStage = souvenirPackage->tournamentStage;
            skin.tournamentTeam1 = souvenirPackage->tournamentTeam1;
            skin.tournamentTeam2 = souvenirPackage->tournamentTeam2;
            skin.proPlayer = souvenirPackage->proPlayer;
            skin.stickers = generateSouvenirStickers(unlockedItem.getWeaponID(), gameItemLookup.getStorage().getTournamentEventID(caseItem.gameItem()), gameItemLookup.getStorage().getTournamentMap(caseItem.gameItem()), skin.tournamentTeam1, skin.tournamentTeam2, skin.proPlayer);
        } else if (attributeGenerator.generateStatTrak()) {
            skin.statTrak = 0;
        }

        return skin;
    }

    [[nodiscard]] inventory::SkinStickers generateSouvenirStickers(WeaponId weaponID, csgo::Tournament tournament, TournamentMap map, csgo::TournamentTeam team1, csgo::TournamentTeam team2, csgo::ProPlayer player) const
    {
        inventory::SkinStickers stickers;

        stickers[0].stickerID = generateTournamentEventStickerID(tournament);

        if (tournament >= csgo::Tournament::EmsOneKatowice2014) {
            stickers[1].stickerID = gameItemLookup.findTournamentTeamGoldStickerID(tournament, team1);
            stickers[2].stickerID = gameItemLookup.findTournamentTeamGoldStickerID(tournament, team2);

            if (player != csgo::ProPlayer{})
                stickers[3].stickerID = gameItemLookup.findTournamentPlayerGoldStickerID(tournament, player);
            else if (tournament >= csgo::Tournament::PglStockholm2021)
                stickers[3].stickerID = game_integration::getTournamentMapGoldStickerID(map);
        }

        attributeGenerator.shuffleStickers(stickerSlotCountGetter(weaponID), stickers);
        return stickers;
    }

    [[nodiscard]] csgo::StickerId generateTournamentEventStickerID(csgo::Tournament tournament) const
    {
        switch (tournament) {
        using enum csgo::StickerId;
        using enum csgo::Tournament;
        case DreamHack2013:
            return attributeGenerator.randomStickerId(Shooter, FrostyTheHitmanFoil);
        case EmsOneKatowice2014:
            return attributeGenerator.randomStickerId(GoldEslWolfFoilKatowice2014, GoldEslSkullFoilKatowice2014);
        case EslOneCologne2014:
            return EslOneCologne2014Gold;
        default:
            return gameItemLookup.findTournamentEventStickerID(tournament);
        }        
    }

    [[nodiscard]] inventory::Gloves generateGloves(const game_items::Item& unlockedItem) const
    {
        const auto& paintKit = gameItemLookup.getStorage().getPaintKit(unlockedItem);

        return inventory::Gloves{
            .wear = std::lerp(paintKit.wearRemapMin, paintKit.wearRemapMax, attributeGenerator.generatePaintKitWear()),
            .seed = attributeGenerator.generatePaintKitSeed()
        };
    }

    const game_items::Lookup& gameItemLookup;
    AttributeGenerator attributeGenerator;
    StickerSlotCountGetter stickerSlotCountGetter;
};

}
