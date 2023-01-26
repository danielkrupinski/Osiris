#include <string_view>

#include <gtest/gtest.h>

#include <MemorySearch/BytePatternStorage.h>

namespace
{

using namespace std::string_view_literals;

TEST(BytePatternStorageTest, StorageHasSizeEqualToLengthOfConvertedPattern) {
    constexpr BytePatternStorage storage{ "83 EC ? ? 4C ? 80 3D" };
    EXPECT_EQ(storage.size, 8);
}

TEST(BytePatternStorageTest, PatternConversionIsPerformedInConstructor) {
    constexpr BytePatternStorage storage{ "E8 ? ? ? ? 48 00 C0 74 62" };
    constexpr auto expectedPattern = "\xE8????\x48\x00\xC0\x74\x62"sv;
    const auto actualPattern = std::string_view{ storage.pattern.data(), storage.size };
    EXPECT_EQ(actualPattern, expectedPattern);
}

TEST(BytePatternStorageTest, StorageCapacityCanBeShrunk) {
    constexpr BytePatternStorage storage{ "A1 ? ? ? ? 89 45" };
    constexpr BytePatternStorage<storage.size> shrunkStorage{ storage };

    constexpr auto expectedPattern = "\xA1????\x89\x45"sv;
    const auto actualPattern = std::string_view{ shrunkStorage.pattern.data(), shrunkStorage.size };
    EXPECT_EQ(actualPattern, expectedPattern);
}

}
