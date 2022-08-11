#include <cstdint>
#include <random>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <InventoryChanger/ItemGenerator/AttributeGenerator.h>

namespace inventory_changer::item_generator
{
namespace
{

class MockRandomGenerator {
public:
    MOCK_METHOD(int, randomInt, (int min, int max));

    template <typename T>
    auto random(T min, T max)
    {
        if constexpr (std::is_same_v<T, int>)
            return randomInt(min, max); 
        else
            static_assert(!std::is_same_v<T, T>, "Unsupported type!");
    }
};

MATCHER_P(LessThanByN, n, "") {
    const auto first = std::get<0>(arg);
    const auto second = std::get<1>(arg);
    return second - first == n;
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
    EXPECT_CALL(randomGenerator, randomInt(testing::_, testing::_)).With(LessThanByN(999)).WillOnce(testing::ReturnArg<0>())
                                                                                          .WillOnce(testing::ReturnArg<1>());
    EXPECT_NE(generatePaintKitSeed(), generatePaintKitSeed());
}

}
}
