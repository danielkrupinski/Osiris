#include <concepts>
#include <limits>
#include <type_traits>

#include <gtest/gtest.h>

#include <Utils/FlagsBuilder.h>

namespace
{

template <std::integral T>
[[nodiscard]] constexpr auto maxValue() noexcept
{
    if constexpr (std::is_unsigned_v<T>) {
        return (std::numeric_limits<T>::max)();
    } else {
        return static_cast<T>(-1);
    }
}

template <typename T>
class Utils_FlagBuilderTest : public testing::Test {
protected:
    template <T flag>
    decltype(auto) set(bool b) noexcept
    {
        return builder.template set<flag>(b);
    }

    decltype(auto) setDummyFlag(bool b) noexcept
    {
        return set<dummyFlag>(b);
    }

    [[nodiscard]] T get() noexcept
    {
        return builder.get();
    }

    static constexpr T firstFlag{ 1 << 0 };
    static constexpr T secondFlag{ 1 << 1 };
    static constexpr auto dummyFlag = maxValue<T>();

private:
    FlagsBuilder<T, T> builder;
};

using Types = testing::Types<
    signed char, unsigned char,
    signed short, unsigned short,
    signed int, unsigned int,
    signed long, unsigned long,
    signed long long, unsigned long long
>;

TYPED_TEST_SUITE(Utils_FlagBuilderTest, Types);

TYPED_TEST(Utils_FlagBuilderTest, HasValueOfZeroWhenConstructed) {
    EXPECT_EQ(this->get(), static_cast<TypeParam>(0));
}

TYPED_TEST(Utils_FlagBuilderTest, FlagIsSetWhenPassingTrue) {
    this->setDummyFlag(true);
    EXPECT_EQ(this->get(), maxValue<TypeParam>());
}

TYPED_TEST(Utils_FlagBuilderTest, FlagIsNotSetWhenPassingFalse) {
    this->setDummyFlag(false);
    EXPECT_EQ(this->get(), static_cast<TypeParam>(0));
}

TYPED_TEST(Utils_FlagBuilderTest, FlagRemainsSetWhenTryingToSetAgain) {
    this->setDummyFlag(true);
    this->setDummyFlag(true);
    EXPECT_EQ(this->get(), this->dummyFlag);
}

TYPED_TEST(Utils_FlagBuilderTest, FlagRemainsSetWhenPassingFalseAfterSetting) {
    this->setDummyFlag(true);
    this->setDummyFlag(false);
    EXPECT_EQ(this->get(), this->dummyFlag);
}

TYPED_TEST(Utils_FlagBuilderTest, MultipleIndependentFlagsCanBeSet) {
    this->template set<TestFixture::firstFlag>(true);
    this->template set<TestFixture::secondFlag>(true);
    EXPECT_EQ(this->get(), TestFixture::firstFlag | TestFixture::secondFlag);
}

TYPED_TEST(Utils_FlagBuilderTest, SetMethodReturnsTheReferenceToSelf) {
    this->template set<TestFixture::firstFlag>(true).template set<TestFixture::secondFlag>(true);
    EXPECT_EQ(this->get(), TestFixture::firstFlag | TestFixture::secondFlag);
}

}
