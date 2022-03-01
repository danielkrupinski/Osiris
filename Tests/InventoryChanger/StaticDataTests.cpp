#include <algorithm>
#include <iomanip>
#include <ostream>
#include <string_view>

#include <gtest/gtest.h>

#include "../../Osiris/InventoryChanger/GameItems/Item.h"
#include "../../Osiris/InventoryChanger/StaticData.h"
#include "../../Osiris/SDK/ItemSchema.h"

class EconRaritiesTest : public testing::TestWithParam<EconRarity> {};

TEST(EconRarities, DefaultConstructedHasNoRarity) {
    ASSERT_EQ(StaticData::EconRarities{}.count(), 0);
}

TEST_P(EconRaritiesTest, SettingUnsetRarityIncrementsCount) {
    StaticData::EconRarities rarities;
    rarities.set(GetParam());
    ASSERT_EQ(rarities.count(), 1);
}

INSTANTIATE_TEST_SUITE_P(, EconRaritiesTest, testing::Values(EconRarity::Default, EconRarity::Purple, EconRarity::Gold));

using StaticData::TournamentMap;

struct TournamentMapTestParam {
    std::string_view lootListName;
    TournamentMap expectedMap;

    friend std::ostream& operator<<(std::ostream& os, const TournamentMapTestParam& param)
    {
        return os << std::quoted(param.lootListName);
    }
};

class GetTournamentMapOfSouvenirPackageTest : public testing::TestWithParam<TournamentMapTestParam> {};

TEST_P(GetTournamentMapOfSouvenirPackageTest, ReturnsExpectedValue) {
    ASSERT_EQ(static_cast<std::uint32_t>(StaticData::getTournamentMapOfSouvenirPackage(GetParam().lootListName)),
              static_cast<std::uint32_t>(GetParam().expectedMap));
}

#define TournamentMapTestParams(name, map) \
TournamentMapTestParam{ name, map }, \
TournamentMapTestParam{ name "_", TournamentMap::None }

INSTANTIATE_TEST_SUITE_P(
    GetTournamentMapOfSouvenirPackageTests,
    GetTournamentMapOfSouvenirPackageTest,
    ::testing::Values(
        TournamentMapTestParam{ {}, TournamentMap::None },
        TournamentMapTestParams("", TournamentMap::None),
        TournamentMapTestParams("de_ancient", TournamentMap::Ancient),
        TournamentMapTestParams("de_cache", TournamentMap::Cache),
        TournamentMapTestParams("de_cbble", TournamentMap::Cobblestone),
        TournamentMapTestParams("de_dust2", TournamentMap::Dust2),
        TournamentMapTestParams("de_inferno", TournamentMap::Inferno),
        TournamentMapTestParams("de_mirage", TournamentMap::Mirage),
        TournamentMapTestParams("de_nuke", TournamentMap::Nuke),
        TournamentMapTestParams("de_overpass", TournamentMap::Overpass),
        TournamentMapTestParams("de_train", TournamentMap::Train),
        TournamentMapTestParams("de_vertigo", TournamentMap::Vertigo)
));
