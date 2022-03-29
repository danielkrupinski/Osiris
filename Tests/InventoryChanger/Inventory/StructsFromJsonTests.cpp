#include <nlohmann/json.hpp>
#include <gtest/gtest.h>

#include <InventoryChanger/Inventory/StructsFromJson.h>

namespace inventory
{
namespace
{

TEST(InventoryChanger_Inventory_GloveFromJsonTests, GloveFromEmptyJsonHasDefaultWear) {
    ASSERT_FLOAT_EQ(gloveFromJson({}).wear, Glove{}.wear);
}

TEST(InventoryChanger_Inventory_GloveFromJsonTests, GloveFromEmptyJsonHasDefaultSeed) {
    ASSERT_EQ(gloveFromJson({}).seed, Glove{}.seed);
}

TEST(InventoryChanger_Inventory_GloveFromJsonTests, GloveFromJsonWithoutWearHasDefaultWear) {
    ASSERT_FLOAT_EQ(gloveFromJson({ { "key", "value" } }).wear, Glove{}.wear);
}

TEST(InventoryChanger_Inventory_GloveFromJsonTests, GloveFromJsonWithoutSeedHasDefaultSeed) {
    ASSERT_EQ(gloveFromJson({ { "key", "value" } }).seed, Glove{}.seed);
}

}
}
