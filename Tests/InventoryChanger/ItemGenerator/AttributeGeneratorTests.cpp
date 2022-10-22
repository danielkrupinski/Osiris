#include <cstdint>
#include <limits>
#include <random>
#include <type_traits>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <InventoryChanger/ItemGenerator/AttributeGenerator.h>

namespace inventory_changer::item_generator
{
namespace
{

class MockRandomGenerator {
public:
    MOCK_METHOD(int, randomFromDistribution, (const std::uniform_int_distribution<int>& dist));

    template <typename Distribution>
    auto operator()(Distribution&& distribution)
    {
        return randomFromDistribution(distribution);
    }
};

MATCHER_P(LessThanByN, n, "") {
    const auto first = std::get<0>(arg);
    const auto second = std::get<1>(arg);
    static_assert(std::is_same_v<decltype(first), decltype(second)>);

    const auto overflow = (first > 0 && second < std::numeric_limits<decltype(first)>::min() + first) ||
                          (first < 0 && second > std::numeric_limits<decltype(first)>::max() + first);
    return !overflow && second - first == n;
}

class InventoryChanger_ItemGenerator_AttributeGeneratorTest : public testing::Test {
protected:
    int generatePaintKitSeed()
    {
        const auto seed = attributeGenerator.generatePaintKitSeed();
        EXPECT_THAT(seed, testing::AllOf(testing::Ge(1), testing::Le(1000)));
        return seed;
    }

    MockRandomGenerator randomGenerator;
    AttributeGenerator<MockRandomGenerator> attributeGenerator{ randomGenerator };
};

TEST_F(InventoryChanger_ItemGenerator_AttributeGeneratorTest, DifferentPaintKitSeedsAreGeneratedForDifferentRandomNumbers) {
    EXPECT_CALL(randomGenerator, randomFromDistribution(std::uniform_int_distribution<int>{ 1, 1000 })).WillOnce(testing::Return(1))
                                                                                                       .WillOnce(testing::Return(1000));
    EXPECT_NE(generatePaintKitSeed(), generatePaintKitSeed());
}

}
}
