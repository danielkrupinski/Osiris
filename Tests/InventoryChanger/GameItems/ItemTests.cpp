#include <gtest/gtest.h>

#include "../../../Osiris/InventoryChanger/GameItems/Item.h"
#include "../../../Osiris/SDK/ItemSchema.h"

class ItemRarityTest : public testing::TestWithParam<EconRarity> {};
