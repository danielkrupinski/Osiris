#include <iterator>
#include <memory>

#include <gtest/gtest.h>

#include <InventoryChanger/Backend/Item.h>
#include <InventoryChanger/GameItems/Item.h>

namespace inventory_changer::backend
{
namespace
{

TEST(InventoryChanger_Backend_ItemConstRemoverTest, ReferenceToTheItemCanBeRetrieved) {
    game_items::Item dummyGameItem{ {}, {}, {}, {}, {} };
    ItemList itemList;
    itemList.emplace_back(dummyGameItem);

    ItemIterator item = std::prev(itemList.end());
    EXPECT_EQ(std::addressof(ItemConstRemover{ itemList }(item)), std::to_address(item));
}

}
}
