#pragma once

#include <list>

namespace inventory
{
    class Item;
}

namespace inventory_changer::backend
{

using ItemList = std::list<inventory::Item>;
using ItemIterator = ItemList::iterator;
using ItemConstIterator = ItemList::const_iterator;

}
