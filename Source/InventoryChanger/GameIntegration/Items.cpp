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

}
