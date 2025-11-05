#include <concepts>
#include <limits>
#include <gtest/gtest.h>
#include <Utils/EnumFlags.h>

template <std::integral T>
constexpr auto kHighestBitMask = static_cast<T>(T{1} << (std::numeric_limits<T>::digits + std::numeric_limits<T>::is_signed - 1));

enum class EnumChar : char {
    First = 1 << 0,
    Another = 1 << 5,
    Last = kHighestBitMask<char>
};

enum class EnumSignedChar : signed char {
    First = 1 << 0,
    Another = 1 << 5,
    Last = kHighestBitMask<signed char>
};

enum class EnumUnsignedChar : unsigned char {
    First = 1 << 0,
    Another = 1 << 5,
    Last = kHighestBitMask<unsigned char>
};

enum class EnumChar8 : char8_t {
    First = 1 << 0,
    Another = 1 << 5,
    Last = kHighestBitMask<char8_t>
};

enum class EnumChar16 : char16_t {
    First = 1 << 0,
    Another = 1 << 12,
    Last = kHighestBitMask<char16_t>
};

enum class EnumChar32 : char32_t {
    First = 1 << 0,
    Another = 1 << 25,
    Last = kHighestBitMask<char32_t>
};

enum class EnumWchar : wchar_t {
    First = 1 << 0,
    Another = 1 << 5,
    Last = kHighestBitMask<wchar_t>
};

enum class EnumSignedShort : signed short {
    First = 1 << 0,
    Another = 1 << 14,
    Last = kHighestBitMask<signed short>
};

enum class EnumUnsignedShort : unsigned short {
    First = 1 << 0,
    Another = 1 << 13,
    Last = kHighestBitMask<unsigned short>
};

enum class EnumSignedInt : signed int {
    First = 1 << 0,
    Another = 1 << 25,
    Last = kHighestBitMask<signed int>
};

enum class EnumUnsignedInt : unsigned int {
    First = 1 << 0,
    Another = 1 << 30,
    Last = kHighestBitMask<unsigned int>
};

enum class EnumSignedLong : signed long {
    First = 1 << 0,
    Another = 1 << 12,
    Last = kHighestBitMask<signed long>
};

enum class EnumUnsignedLong : unsigned long {
    First = 1 << 0,
    Another = 1 << 13,
    Last = kHighestBitMask<unsigned long>
};

enum class EnumSignedLongLong : signed long long {
    First = 1 << 0,
    Another = 1LL << 41,
    Last = kHighestBitMask<signed long long>
};

enum class EnumUnsignedLongLong : unsigned long long {
    First = 1 << 0,
    Another = 1ULL << 46,
    Last = kHighestBitMask<unsigned long long>
};

template <typename Enum>
class EnumFlagsTest : public testing::Test {
};

using EnumFlagsTestTypes = testing::Types<
    EnumChar,
    EnumSignedChar,
    EnumUnsignedChar,
    EnumChar8,
    EnumChar16,
    EnumChar32,
    EnumWchar,
    EnumSignedShort,
    EnumUnsignedShort,
    EnumSignedInt,
    EnumUnsignedInt,
    EnumSignedLong,
    EnumUnsignedLong,
    EnumSignedLongLong,
    EnumUnsignedLongLong
>;
TYPED_TEST_SUITE(EnumFlagsTest, EnumFlagsTestTypes);

TYPED_TEST(EnumFlagsTest, DefaultConstructedHasNoFlags) {
    EnumFlags<TypeParam> enumFlags;
    EXPECT_FALSE(enumFlags.has(TypeParam::First));
    EXPECT_FALSE(enumFlags.has(TypeParam::Another));
    EXPECT_FALSE(enumFlags.has(TypeParam::Last));
    EXPECT_FALSE(enumFlags.hasAny());
}

TYPED_TEST(EnumFlagsTest, FlagsCanBeSet) {
    EnumFlags<TypeParam> enumFlags;

    enumFlags.set(TypeParam::First);
    EXPECT_TRUE(enumFlags.has(TypeParam::First));
    EXPECT_TRUE(enumFlags.hasAny());

    enumFlags.set(TypeParam::Another);
    EXPECT_TRUE(enumFlags.has(TypeParam::Another));
    EXPECT_TRUE(enumFlags.hasAny());

    enumFlags.set(TypeParam::Last);
    EXPECT_TRUE(enumFlags.has(TypeParam::Last));
    EXPECT_TRUE(enumFlags.hasAny());
}

TYPED_TEST(EnumFlagsTest, SettingAlreadySetFlagDoesNothing) {
    EnumFlags<TypeParam> enumFlags;

    enumFlags.set(TypeParam::First);
    enumFlags.set(TypeParam::First);
    EXPECT_TRUE(enumFlags.has(TypeParam::First));
    EXPECT_TRUE(enumFlags.hasAny());

    enumFlags.set(TypeParam::Another);
    enumFlags.set(TypeParam::Another);
    EXPECT_TRUE(enumFlags.has(TypeParam::Another));
    EXPECT_TRUE(enumFlags.hasAny());

    enumFlags.set(TypeParam::Last);
    enumFlags.set(TypeParam::Last);
    EXPECT_TRUE(enumFlags.has(TypeParam::Last));
    EXPECT_TRUE(enumFlags.hasAny());
}

TYPED_TEST(EnumFlagsTest, AllFlagsCanBeSetAtOnce) {
    EnumFlags<TypeParam> enumFlags;

    enumFlags.setAll();
    EXPECT_TRUE(enumFlags.has(TypeParam::First));
    EXPECT_TRUE(enumFlags.has(TypeParam::Another));
    EXPECT_TRUE(enumFlags.has(TypeParam::Last));
    EXPECT_TRUE(enumFlags.hasAny());

    enumFlags.clear();
    enumFlags.set(TypeParam::First);
    enumFlags.setAll();
    EXPECT_TRUE(enumFlags.has(TypeParam::First));
    EXPECT_TRUE(enumFlags.has(TypeParam::Another));
    EXPECT_TRUE(enumFlags.has(TypeParam::Last));
    EXPECT_TRUE(enumFlags.hasAny());
}

TYPED_TEST(EnumFlagsTest, FlagsCanBeCleared) {
    EnumFlags<TypeParam> enumFlags;

    enumFlags.setAll();
    enumFlags.clear();
    EXPECT_FALSE(enumFlags.hasAny());

    enumFlags.set(TypeParam::First);
    enumFlags.clear();
    EXPECT_FALSE(enumFlags.hasAny());
}
