#pragma once

#include <cassert>
#include <memory>
#include <variant>
#include <vector>

#include <InventoryChanger/GameItems/Item.h>

#include "Item.h"
#include "Structs.h"

namespace inventory
{

class Storage {
public:
    void addSkin(const game_items::Item& gameItem, const Skin& skin)
    {
        assert(gameItem.isSkin());
        items.emplace_back(gameItem, std::make_unique<Skin>(skin));
    }

    void addGloves(const game_items::Item& gameItem, const Glove& gloves)
    {
        assert(gameItem.isGloves());
        items.emplace_back(gameItem, std::make_unique<Glove>(gloves));
    }

    void addAgent(const game_items::Item& gameItem, const Agent& agent)
    {
        assert(gameItem.isAgent());
        items.emplace_back(gameItem, std::make_unique<Agent>(agent));
    }

    void addMusic(const game_items::Item& gameItem, const Music& music)
    {
        assert(gameItem.isMusic());
        items.emplace_back(gameItem, std::make_unique<Music>(music));
    }

    [[nodiscard]] Skin& getSkin(const Item_v2& item)
    {
        assert(item.gameItem().isSkin());
        return *std::get<std::unique_ptr<Skin>>(item.getData()).get();
    }

    [[nodiscard]] Glove& getGloves(const Item_v2& item)
    {
        assert(item.gameItem().isGloves());
        return *std::get<std::unique_ptr<Glove>>(item.getData()).get();
    }

    [[nodiscard]] Agent& getAgent(const Item_v2& item)
    {
        assert(item.gameItem().isAgent());
        return *std::get<std::unique_ptr<Agent>>(item.getData()).get();
    }

    [[nodiscard]] auto begin()
    {
        return items.begin();
    }

    [[nodiscard]] auto end()
    {
        return items.end();
    }

    std::vector<Item_v2>::iterator removeItem(std::vector<Item_v2>::iterator it)
    {
        return items.erase(it);
    }

private:
    std::vector<Item_v2> items;
};

}
