#include "Items.h"

#include <Interfaces.h>

namespace inventory_changer::game_integration
{

void Items::getMusicKits(game_items::Storage& storage)
{
    for (const auto& node : itemSchema.musicKits) {
        const auto musicKit = node.value;
        if (musicKit->id == 1 || musicKit->id == 2)
            continue;

        const auto musicName = interfaces->localize->findSafe(musicKit->nameLocalized);
        storage.addMusic(musicKit->id, game_items::ItemName{ toUtf8.convertUnicodeToAnsi(musicName), toUpperConverter.toUpper(musicName) }, musicKit->inventoryImage);
    }
}

void Items::getStickers(game_items::Storage& storage)
{
    const auto& stickerMap = itemSchema.stickerKits;
    for (const auto& node : stickerMap) {
        const auto stickerKit = node.value;
        if (stickerKit->id == 0)
            continue;

        const auto name = std::string_view{ stickerKit->name.data(), static_cast<std::size_t>(stickerKit->name.length - 1) };
        const auto isPatch = name.starts_with("patch") || name.starts_with("stockh2021_teampatch");
        const auto isGraffiti = !isPatch && (name.starts_with("spray") || name.ends_with("graffiti"));
        const auto isSticker = !isPatch && !isGraffiti;

        if (isSticker) {
            const auto isGolden = name.ends_with("gold");
            const auto stickerName = interfaces->localize->findSafe(stickerKit->id != 242 ? stickerKit->itemName.data() : "StickerKit_dhw2014_teamdignitas_gold");
            storage.addSticker(stickerKit->id, game_items::ItemName{ toUtf8.convertUnicodeToAnsi(stickerName), toUpperConverter.toUpper(stickerName) }, static_cast<EconRarity>(stickerKit->rarity), stickerKit->inventoryImage.data(), stickerKit->tournamentID, static_cast<TournamentTeam>(stickerKit->tournamentTeamID), stickerKit->tournamentPlayerID, isGolden);
        } else if (isPatch) {
            const auto patchName = interfaces->localize->findSafe(stickerKit->itemName.data());
            storage.addPatch(stickerKit->id, game_items::ItemName{ toUtf8.convertUnicodeToAnsi(patchName), toUpperConverter.toUpper(patchName) }, static_cast<EconRarity>(stickerKit->rarity), stickerKit->inventoryImage.data());
        } else if (isGraffiti) {
            const auto paintName = interfaces->localize->findSafe(stickerKit->itemName.data());
            storage.addGraffiti(stickerKit->id, game_items::ItemName{ toUtf8.convertUnicodeToAnsi(paintName), toUpperConverter.toUpper(paintName) }, static_cast<EconRarity>(stickerKit->rarity), stickerKit->inventoryImage.data());
        }
    }
}

}
