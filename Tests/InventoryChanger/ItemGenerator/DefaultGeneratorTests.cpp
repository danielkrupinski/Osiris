#include <chrono>

#include <gtest/gtest.h>

#include <InventoryChanger/GameItems/Item.h>
#include <InventoryChanger/GameItems/Storage.h>
#include <InventoryChanger/ItemGenerator/DefaultGenerator.h>

#include <CSGO/ItemSchema.h>

namespace inventory_changer::item_generator
{
namespace
{

class FakeSystemClock {
public:
    using duration = std::chrono::seconds;
    using period = duration::period;
    using rep = duration::rep;
    using time_point = std::chrono::time_point<FakeSystemClock>;

    static constexpr bool is_steady = false;

    [[nodiscard]] static time_point now() noexcept
    {
        return time;
    }

    [[nodiscard]] static std::time_t to_time_t(const time_point& t) noexcept
    {
        return t.time_since_epoch().count();
    } 

    [[nodiscard]] static time_point from_time_t(std::time_t t) noexcept
    {
        return time_point{ std::chrono::seconds{ t } };
    } 

    static void set(time_point t)
    {
        time = t;
    }

    static void set(std::time_t t)
    {
        time = from_time_t(t);
    }

private:
    static inline time_point time;
};

class MockAttributeGenerator {};

class InventoryChanger_ItemGenerator_DefaultGeneratorTest : public testing::Test {

protected:
    game_items::Storage gameItemStorage;
    MockAttributeGenerator attributeGenerator;
    DefaultGenerator<const MockAttributeGenerator&, FakeSystemClock> defaultGenerator{ gameItemStorage, attributeGenerator };
};

struct TradePenalty {
    std::time_t now;
    std::time_t end;
};

class InventoryChanger_ItemGenerator_DefaultGenerator_TradePenaltyTest
    : public InventoryChanger_ItemGenerator_DefaultGeneratorTest, public testing::WithParamInterface<TradePenalty> {};

TEST_P(InventoryChanger_ItemGenerator_DefaultGenerator_TradePenaltyTest, CrateKeysAreNotTradableForOneWeek) {
    gameItemStorage.addCaseKey(EconRarity::Default, WeaponId{}, {});
    const auto& key = gameItemStorage.getItems().back();
    FakeSystemClock::set(GetParam().now);
    const auto commonProperties = defaultGenerator.createCommonProperties(key);
    ASSERT_EQ(commonProperties.tradableAfterDate, GetParam().end);
}

TEST_P(InventoryChanger_ItemGenerator_DefaultGenerator_TradePenaltyTest, IemRio2022CratesAreNotTradableForOneWeek) {
    gameItemStorage.addCrate(EconRarity::Default, WeaponId{}, 0, csgo::Tournament::IemRio2022, {}, false, {});
    const auto& crate = gameItemStorage.getItems().back();
    FakeSystemClock::set(GetParam().now);
    const auto commonProperties = defaultGenerator.createCommonProperties(crate);
    ASSERT_EQ(commonProperties.tradableAfterDate, GetParam().end);
}

INSTANTIATE_TEST_SUITE_P(, InventoryChanger_ItemGenerator_DefaultGenerator_TradePenaltyTest, testing::Values(
    TradePenalty{ 1660629977, 1661238000 },
    TradePenalty{ 1660808922, 1661497200 }
));

}
}
