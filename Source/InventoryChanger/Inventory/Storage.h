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
        items.emplace_back(gameItem, skin);
    }

    void addGloves(const game_items::Item& gameItem, const Glove& gloves)
    {
        assert(gameItem.isGloves());
        items.emplace_back(gameItem, gloves);
    }

    void addAgent(const game_items::Item& gameItem, const Agent& agent)
    {
        assert(gameItem.isAgent());
        items.emplace_back(gameItem, agent);
    }

    void addMusic(const game_items::Item& gameItem, const Music& music)
    {
        assert(gameItem.isMusic());
        items.emplace_back(gameItem, music);
    }

    void addGraffiti(const game_items::Item& gameItem, const Graffiti& graffiti)
    {
        assert(gameItem.isGraffiti());
        items.emplace_back(gameItem, graffiti);
    }

    void addServiceMedal(const game_items::Item& gameItem, const ServiceMedal& serviceMedal)
    {
        assert(gameItem.isServiceMedal());
        items.emplace_back(gameItem, serviceMedal);
    }

    void addSouvenirPackage(const game_items::Item& gameItem, const SouvenirPackage& souvenirPackage)
    {
        assert(gameItem.isCase());
        items.emplace_back(gameItem, souvenirPackage);
    }

    [[nodiscard]] auto begin()
    {
        return items.begin();
    }

    [[nodiscard]] auto end()
    {
        return items.end();
    }

    [[nodiscard]] auto begin() const
    {
        return items.begin();
    }

    [[nodiscard]] auto end() const
    {
        return items.end();
    }

    std::vector<Item_v2>::iterator removeItem(std::vector<Item_v2>::iterator it)
    {
        return items.erase(it);
    }

    void clear()
    {
        items.clear();
    }

private:
    std::vector<Item_v2> items;
};

}
