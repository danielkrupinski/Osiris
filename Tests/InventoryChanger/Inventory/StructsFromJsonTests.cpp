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

TEST(InventoryChanger_Inventory_GloveFromJsonTests, GloveFromJsonWithoutWearHasDefaultWear) {
    ASSERT_FLOAT_EQ(gloveFromJson({ { "key", "value" } }).wear, Glove{}.wear);
}

class InventoryChanger_Inventory_GloveFromJsonWearTypeTest : public testing::TestWithParam<json::value_t> {};

TEST_P(InventoryChanger_Inventory_GloveFromJsonWearTypeTest, GloveFromJsonWithWearNotAsFloatHasDefaultWear) {
    ASSERT_FLOAT_EQ(gloveFromJson({ { "Wear", GetParam() } }).wear, Glove{}.wear);
}

INSTANTIATE_TEST_SUITE_P(,
    InventoryChanger_Inventory_GloveFromJsonWearTypeTest,
    testing::Values(json::value_t::array,
                    json::value_t::binary,
                    json::value_t::boolean,
                    json::value_t::discarded,
                    json::value_t::null,
                    json::value_t::number_integer,
                    json::value_t::number_unsigned,
                    json::value_t::object,
                    json::value_t::string)
);

class InventoryChanger_Inventory_GloveFromJsonWearTest : public testing::TestWithParam<float> {};

TEST_P(InventoryChanger_Inventory_GloveFromJsonWearTest, GloveFromJsonHasCorrectWear) {
    ASSERT_FLOAT_EQ(gloveFromJson({ { "Wear", GetParam() } }).wear, GetParam());
}

INSTANTIATE_TEST_SUITE_P(, InventoryChanger_Inventory_GloveFromJsonWearTest, testing::Values(0.0f, 0.123f, 1.0f));

TEST(InventoryChanger_Inventory_GloveFromJsonTests, GloveFromEmptyJsonHasDefaultSeed) {
    ASSERT_EQ(gloveFromJson({}).seed, Glove{}.seed);
}

TEST(InventoryChanger_Inventory_GloveFromJsonTests, GloveFromJsonWithoutSeedHasDefaultSeed) {
    ASSERT_EQ(gloveFromJson({ { "key", "value" } }).seed, Glove{}.seed);
}

class InventoryChanger_Inventory_GloveFromJsonSeedTypeTest : public testing::TestWithParam<json::value_t> {};

TEST_P(InventoryChanger_Inventory_GloveFromJsonSeedTypeTest, GloveFromJsonWithSeedNotAsIntegerHasDefaultSeed) {
    ASSERT_EQ(gloveFromJson({ { "Seed", GetParam() } }).seed, Glove{}.seed);
}

INSTANTIATE_TEST_SUITE_P(,
    InventoryChanger_Inventory_GloveFromJsonSeedTypeTest,
    testing::Values(json::value_t::array,
                    json::value_t::binary,
                    json::value_t::boolean,
                    json::value_t::discarded,
                    json::value_t::null,
                    json::value_t::number_float,
                    json::value_t::object,
                    json::value_t::string)
);

class InventoryChanger_Inventory_GloveFromJsonSeedTest : public testing::TestWithParam<int> {};

TEST_P(InventoryChanger_Inventory_GloveFromJsonSeedTest, GloveFromJsonHasCorrectSeed) {
    ASSERT_EQ(gloveFromJson({ { "Seed", GetParam() } }).seed, GetParam());
}

INSTANTIATE_TEST_SUITE_P(, InventoryChanger_Inventory_GloveFromJsonSeedTest, testing::Values(0, 123, 1000));

class InventoryChanger_Inventory_GloveFromJsonAllParamsTest : public testing::TestWithParam<std::tuple<float, int>> {};

TEST_P(InventoryChanger_Inventory_GloveFromJsonAllParamsTest, GloveFromJsonHasCorrectWear) {
    const auto [wear, seed] = GetParam();
    ASSERT_FLOAT_EQ(gloveFromJson({ { "Wear", wear }, { "Seed", seed } }).wear, wear);
}

TEST_P(InventoryChanger_Inventory_GloveFromJsonAllParamsTest, GloveFromJsonHasCorrectSeed) {
    const auto [wear, seed] = GetParam();
    ASSERT_EQ(gloveFromJson({ { "Wear", wear }, { "Seed", seed } }).seed, seed);
}

INSTANTIATE_TEST_SUITE_P(, InventoryChanger_Inventory_GloveFromJsonAllParamsTest,
    testing::Combine(
        testing::Values(0.0f, 0.123f, 1.0f),
        testing::Values(0, 123, 1000)
));

TEST(InventoryChanger_Inventory_SouvenirPackageFromJsonTests, SouvenirPackageFromEmptyJsonHasDefaultTournamentStage) {
    ASSERT_EQ(souvenirPackageFromJson({}).tournamentStage, SouvenirPackage{}.tournamentStage);
}

TEST(InventoryChanger_Inventory_SouvenirPackageFromJsonTests, SouvenirPackageFromEmptyJsonHasDefaultTournamentTeam1) {
    ASSERT_EQ(souvenirPackageFromJson({}).tournamentTeam1, SouvenirPackage{}.tournamentTeam1);
}

TEST(InventoryChanger_Inventory_SouvenirPackageFromJsonTests, SouvenirPackageFromEmptyJsonHasDefaultTournamentTeam2) {
    ASSERT_EQ(souvenirPackageFromJson({}).tournamentTeam2, SouvenirPackage{}.tournamentTeam2);
}

TEST(InventoryChanger_Inventory_SouvenirPackageFromJsonTests, SouvenirPackageFromEmptyJsonHasDefaultProPlayer) {
    ASSERT_EQ(souvenirPackageFromJson({}).proPlayer, SouvenirPackage{}.proPlayer);
}

class InventoryChanger_Inventory_SouvenirPackageFromJsonTypeTest : public testing::TestWithParam<json::value_t> {};

TEST_P(InventoryChanger_Inventory_SouvenirPackageFromJsonTypeTest, FromJsonWithTournamentStageNotAsUintHasDefaultTournamentStage) {
    ASSERT_EQ(souvenirPackageFromJson({ { "Tournament Stage", GetParam() } }).tournamentStage, SouvenirPackage{}.tournamentStage);
}

TEST_P(InventoryChanger_Inventory_SouvenirPackageFromJsonTypeTest, FromJsonWithTournamentTeam1NotAsUintHasDefaultTournamentTeam1) {
    ASSERT_EQ(souvenirPackageFromJson({ { "Tournament Team 1", GetParam() } }).tournamentTeam1, SouvenirPackage{}.tournamentTeam1);
}

TEST_P(InventoryChanger_Inventory_SouvenirPackageFromJsonTypeTest, FromJsonWithTournamentTeam2NotAsUintHasDefaultTournamentTeam2) {
    ASSERT_EQ(souvenirPackageFromJson({ { "Tournament Team 2", GetParam() } }).tournamentTeam2, SouvenirPackage{}.tournamentTeam2);
}

TEST_P(InventoryChanger_Inventory_SouvenirPackageFromJsonTypeTest, FromJsonWithProPlayerNotAsUintHasDefaultProPlayer) {
    ASSERT_EQ(souvenirPackageFromJson({ { "Tournament Player", GetParam() } }).proPlayer, SouvenirPackage{}.proPlayer);
}

INSTANTIATE_TEST_SUITE_P(,
    InventoryChanger_Inventory_SouvenirPackageFromJsonTypeTest,
    testing::Values(json::value_t::array,
                    json::value_t::binary,
                    json::value_t::boolean,
                    json::value_t::discarded,
                    json::value_t::null,
                    json::value_t::number_float,
                    json::value_t::object,
                    json::value_t::string)
);

}
}
