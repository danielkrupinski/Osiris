#pragma once

#include <Helpers.h>
#include <InventoryChanger/GameItems/Storage.h>
#include <SDK/Localize.h>

class ItemSchema;

namespace inventory_changer::game_integration
{

class Items {
public:
    Items(ItemSchema& itemSchema, Localize& localize) : itemSchema{ itemSchema }, toUtf8{ localize } {}

    void getMusicKits(game_items::Storage& storage);
    void getStickers(game_items::Storage& storage);
    void getSkinsAndGloves(game_items::Storage& storage);
    void getOtherItems(game_items::Storage& storage);

private:
    ItemSchema& itemSchema;
    ToUtf8Converter<> toUtf8;
    Helpers::ToUpperConverter toUpperConverter;
};


[[nodiscard]] game_items::Storage createGameItemStorage(Items& items);

}
