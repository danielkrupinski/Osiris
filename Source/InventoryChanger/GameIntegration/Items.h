#pragma once

#include <Helpers.h>
#include <InventoryChanger/GameItems/Storage.h>
#include <CSGO/Localize.h>

namespace csgo { struct ItemSchema; }
class OtherInterfaces;

namespace inventory_changer::game_integration
{

class Items {
public:
    Items(csgo::ItemSchema& itemSchema, const csgo::Localize& localize) : itemSchema{ itemSchema }, toUtf8{ localize } {}

    void getMusicKits(const OtherInterfaces& interfaces, game_items::Storage& storage);
    void getStickers(const OtherInterfaces& interfaces, game_items::Storage& storage);
    void getSkinsAndGloves(const OtherInterfaces& interfaces, game_items::Storage& storage);
    void getOtherItems(game_items::Storage& storage);

private:
    csgo::ItemSchema& itemSchema;
    ToUtf8Converter<> toUtf8;
    Helpers::ToUpperConverter toUpperConverter;
};


[[nodiscard]] game_items::Storage createGameItemStorage(const OtherInterfaces& interfaces, Items& items);

}
