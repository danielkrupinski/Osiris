#include <algorithm>
#include <memory>
#include <string_view>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <Utils/StringBuilder.h>

namespace
{

class TestStringBuilder : public testing::Test {
protected:
    [[nodiscard]] auto bufferString() const noexcept
    {
        return std::string_view{ std::begin(buffer), std::end(buffer) };
    }

    void putIntoBuffer(std::string_view str)
    {
        std::copy_n(str.data(), (std::min)(str.size(), sizeof(buffer)), &buffer[0]);
    }

    char buffer[30]{};
    StringBuilder builder{ buffer };
};

TEST_F(TestStringBuilder, StringIsFormattedIntoProvidedBuffer) {
    builder.put('a', " first ", 1234, [](StringBuilder& builder) { builder.put(567); }, " end");
    EXPECT_TRUE(bufferString().starts_with("a first 1234567 end"));
}

TEST_F(TestStringBuilder, PutMethodReturnsReferenceToSelf) {
    decltype(auto) result = builder.put("str");
    EXPECT_EQ(std::addressof(result), std::addressof(builder));
}

TEST_F(TestStringBuilder, PutHexMethodReturnsReferenceToSelf) {
    decltype(auto) result = builder.putHex(0x123);
    EXPECT_EQ(std::addressof(result), std::addressof(builder));
}

TEST_F(TestStringBuilder, StringViewWithCanBeRetrieved) {
    builder.put("some string ").putHex(0xc0de);
    const auto string = builder.string();
    EXPECT_EQ(string.data(), &buffer[0]);
    EXPECT_EQ(string, "some string c0de");
}

TEST_F(TestStringBuilder, NullTerminatedCstringCanBeRetrieved) {
    putIntoBuffer("abcdef");
    builder.put("str");
    const auto cstring = builder.cstring();
    EXPECT_EQ(cstring, &buffer[0]);
    EXPECT_STREQ(cstring, "str");
}

TEST_F(TestStringBuilder, EmptyStringViewIsReturnedWhenNothingWasWritten) {
    const auto string = builder.string();
    EXPECT_EQ(string.data(), &buffer[0]);
    EXPECT_TRUE(string.empty());
}

TEST_F(TestStringBuilder, EmptyNullTerminatedCstringIsReturnedWhenNothingWasWritten) {
    const auto cstring = builder.cstring();
    EXPECT_EQ(cstring, &buffer[0]);
    EXPECT_EQ(cstring[0], '\0');
}

TEST_F(TestStringBuilder, StringCanBeWritten) {
    builder.put("one").put("two").put("three");
    EXPECT_EQ(builder.string(), "onetwothree");
}

TEST_F(TestStringBuilder, CharCanBeWritten) {
    builder.put('x').put('y').put('z');
    EXPECT_EQ(builder.string(), "xyz");
}

TEST_F(TestStringBuilder, IntegerCanBeWrittenAsDecimal) {
    builder.put(123).put(456).put(777);
    EXPECT_EQ(builder.string(), "123456777");
}

TEST_F(TestStringBuilder, IntegerCanBeWrittenAsHexadecimal) {
    builder.putHex(0xc).putHex(0xabcdef0123456789);
    EXPECT_EQ(builder.string(), "cabcdef0123456789");
}

TEST_F(TestStringBuilder, PutAcceptsInvocable) {
    testing::MockFunction<void(StringBuilder& builder)> mockInvocable;
    EXPECT_CALL(mockInvocable, Call(testing::Ref(builder))).WillOnce(testing::Invoke(
        [](StringBuilder& builder) { builder.put("123"); }
    ));
    builder.put(mockInvocable.AsStdFunction());
    EXPECT_EQ(builder.string(), "123");
}

}
