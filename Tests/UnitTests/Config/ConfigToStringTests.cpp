#include <array>
#include <span>
#include <stdexcept>
#include <string_view>

#include <gtest/gtest.h>

#include <Config/ConfigStringConversionState.h>
#include <Config/ConfigToString.h>

struct ConfigToStringTestParam {
    std::size_t bufferSize{};
    ConfigStringConversionState conversionState{};
    std::u8string_view expectedBufferContent{};
    ConfigStringConversionState expectedConversionState{};
};

using CreateConfigSchemaFunctor = std::function<void(ConfigToString&)>;

class ConfigToStringTest : public testing::TestWithParam<std::tuple<CreateConfigSchemaFunctor, ConfigToStringTestParam>> {
protected:
    [[nodiscard]] bool bufferEquals(std::span<const char8_t> buffer, std::u8string_view str) const
    {
        if (str.size() > buffer.size())
            return false;
        return std::ranges::equal(buffer.first(str.size()), str);
    }

    std::array<char8_t, 4096> buffer{};
};

TEST_P(ConfigToStringTest, TestConfigToStringConversion) {
    const auto& param = std::get<1>(GetParam());

    if (param.bufferSize > buffer.size())
        throw std::invalid_argument{"bufferSize param exceeds buffer size"};
    
    if (param.expectedBufferContent.size() > param.bufferSize)
        throw std::invalid_argument{"size of expected buffer content exceeds bufferSize param"};

    const auto bufferSlice = std::span{buffer}.first(param.bufferSize);
    auto conversionState = param.conversionState;
    ConfigToString configToString{bufferSlice, conversionState};
    configToString.beginRoot();
    std::get<0>(GetParam())(configToString);
    const auto writtenBytes = configToString.endRoot();

    EXPECT_TRUE(bufferEquals(bufferSlice, param.expectedBufferContent));
    std::cout << param.expectedBufferContent.size() << '\n';
    EXPECT_EQ(writtenBytes, param.expectedBufferContent.size());
    ASSERT_EQ(conversionState.nestingLevel, param.expectedConversionState.nestingLevel);
    for (auto nestingLevel = 0u; nestingLevel <= conversionState.nestingLevel; ++nestingLevel)
        EXPECT_EQ(conversionState.indexInNestingLevel[nestingLevel], param.expectedConversionState.indexInNestingLevel[nestingLevel]);
}

INSTANTIATE_TEST_SUITE_P(EmptyRoot, ConfigToStringTest, testing::Combine(
testing::Values(
    [](ConfigToString&){}
),
testing::Values(
    ConfigToStringTestParam{
        .bufferSize = 2,
        .expectedBufferContent = u8"{}",
        .expectedConversionState{
            .nestingLevel = 0,
            .indexInNestingLevel{1}
        }
    },
    ConfigToStringTestParam{
        .bufferSize = 1000,
        .expectedBufferContent = u8"{}",
        .expectedConversionState{
            .nestingLevel = 0,
            .indexInNestingLevel{1}
        }
    },
    ConfigToStringTestParam{
        .bufferSize = 1,
        .expectedBufferContent = u8"{",
        .expectedConversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 0}
        }
    },
    ConfigToStringTestParam{
        .bufferSize = 1,
        .conversionState{
            .nestingLevel = 1
        },
        .expectedBufferContent = u8"}",
        .expectedConversionState{
            .nestingLevel = 0,
            .indexInNestingLevel{1}
        }
    },
    ConfigToStringTestParam{
        .bufferSize = 0,
        .expectedBufferContent = u8"",
        .expectedConversionState{
            .nestingLevel = 0,
            .indexInNestingLevel{0}
        }
    })
));

INSTANTIATE_TEST_SUITE_P(OneObjectInRoot, ConfigToStringTest, testing::Combine(
testing::Values(
    [](ConfigToString& configToString) {
        configToString.beginObject(u8"Obj1");
        configToString.endObject();
    }
),
testing::Values(
    ConfigToStringTestParam{
        .bufferSize = 11,
        .expectedBufferContent = u8"{\"Obj1\":{}}",
        .expectedConversionState{
            .nestingLevel = 0,
            .indexInNestingLevel{1}
        }
    },
    ConfigToStringTestParam{
        .bufferSize = 1000,
        .expectedBufferContent = u8"{\"Obj1\":{}}",
        .expectedConversionState{
            .nestingLevel = 0,
            .indexInNestingLevel{1}
        }
    },
    ConfigToStringTestParam{
        .bufferSize = 1,
        .expectedBufferContent = u8"{",
        .expectedConversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 0}
        }
    },
    ConfigToStringTestParam{
        .bufferSize = 8,
        .expectedBufferContent = u8"{",
        .expectedConversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 0}
        }
    },
    ConfigToStringTestParam{
        .bufferSize = 9,
        .expectedBufferContent = u8"{\"Obj1\":{",
        .expectedConversionState{
            .nestingLevel = 2,
            .indexInNestingLevel{0, 0, 0}
        }
    },
    ConfigToStringTestParam{
        .bufferSize = 1,
        .conversionState{
            .nestingLevel = 2,
            .indexInNestingLevel{0, 0, 0}
        },
        .expectedBufferContent = u8"}",
        .expectedConversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 1}
        }
    },
    ConfigToStringTestParam{
        .bufferSize = 1,
        .conversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 1}
        },
        .expectedBufferContent = u8"}",
        .expectedConversionState{
            .nestingLevel = 0,
            .indexInNestingLevel{1}
        }
    })
));

INSTANTIATE_TEST_SUITE_P(MultipleElementsInRoot, ConfigToStringTest, testing::Combine(
testing::Values(
    [](ConfigToString& configToString) {
        configToString.boolean(u8"BoolFirst", [](bool){}, []{ return false; });
        configToString.beginObject(u8"Obj1");
        configToString.endObject();
        configToString.uint(u8"UintFirst", [](std::uint64_t){}, []{ return 0; });
        configToString.boolean(u8"BoolSecond", [](bool){}, []{ return true; });
        configToString.beginObject(u8"Obj2");
        configToString.endObject();
        configToString.uint(u8"UintSecond", [](std::uint64_t){}, []{ return 123; });
        configToString.uint(u8"UintThird", [](std::uint64_t){}, []{ return (std::numeric_limits<std::uint64_t>::max)(); });
    }
),
testing::Values(
    ConfigToStringTestParam{
        .bufferSize = 121,
        .expectedBufferContent = u8"{\"BoolFirst\":false,\"Obj1\":{},\"UintFirst\":0,\"BoolSecond\":true,\"Obj2\":{},\"UintSecond\":123,\"UintThird\":18446744073709551615}",
        .expectedConversionState{
            .nestingLevel = 0,
            .indexInNestingLevel{1}
        }
    },
    ConfigToStringTestParam{
        .bufferSize = 1000,
        .expectedBufferContent = u8"{\"BoolFirst\":false,\"Obj1\":{},\"UintFirst\":0,\"BoolSecond\":true,\"Obj2\":{},\"UintSecond\":123,\"UintThird\":18446744073709551615}",
        .expectedConversionState{
            .nestingLevel = 0,
            .indexInNestingLevel{1}
        }
    },
    ConfigToStringTestParam{
        .bufferSize = 17,
        .expectedBufferContent = u8"{",
        .expectedConversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 0}
        }
    },
    ConfigToStringTestParam{
        .bufferSize = 18,
        .expectedBufferContent = u8"{\"BoolFirst\":false",
        .expectedConversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 1}
        }
    },
    ConfigToStringTestParam{
        .bufferSize = 8,
        .conversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 1}
        },
        .expectedBufferContent = u8"",
        .expectedConversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 1}
        }
    },
    ConfigToStringTestParam{
        .bufferSize = 9,
        .conversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 1}
        },
        .expectedBufferContent = u8",\"Obj1\":{",
        .expectedConversionState{
            .nestingLevel = 2,
            .indexInNestingLevel{0, 1, 0}
        }
    },
    ConfigToStringTestParam{
        .bufferSize = 1,
        .conversionState{
            .nestingLevel = 2,
            .indexInNestingLevel{0, 1, 0}
        },
        .expectedBufferContent = u8"}",
        .expectedConversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 2}
        }
    },
    ConfigToStringTestParam{
        .bufferSize = 13,
        .conversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 2}
        },
        .expectedBufferContent = u8"",
        .expectedConversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 2}
        }
    },
    ConfigToStringTestParam{
        .bufferSize = 14,
        .conversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 2}
        },
        .expectedBufferContent = u8",\"UintFirst\":0",
        .expectedConversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 3}
        }
    },
    ConfigToStringTestParam{
        .bufferSize = 17,
        .conversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 3}
        },
        .expectedBufferContent = u8"",
        .expectedConversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 3}
        }
    },
    ConfigToStringTestParam{
        .bufferSize = 18,
        .conversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 3}
        },
        .expectedBufferContent = u8",\"BoolSecond\":true",
        .expectedConversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 4}
        }
    },
    ConfigToStringTestParam{
        .bufferSize = 8,
        .conversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 4}
        },
        .expectedBufferContent = u8"",
        .expectedConversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 4}
        }
    },
    ConfigToStringTestParam{
        .bufferSize = 9,
        .conversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 4}
        },
        .expectedBufferContent = u8",\"Obj2\":{",
        .expectedConversionState{
            .nestingLevel = 2,
            .indexInNestingLevel{0, 4, 0}
        }
    },
    ConfigToStringTestParam{
        .bufferSize = 1,
        .conversionState{
            .nestingLevel = 2,
            .indexInNestingLevel{0, 4, 0}
        },
        .expectedBufferContent = u8"}",
        .expectedConversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 5}
        }
    },
    ConfigToStringTestParam{
        .bufferSize = 16,
        .conversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 5}
        },
        .expectedBufferContent = u8"",
        .expectedConversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 5}
        }
    },
    ConfigToStringTestParam{
        .bufferSize = 17,
        .conversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 5}
        },
        .expectedBufferContent = u8",\"UintSecond\":123",
        .expectedConversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 6}
        }
    },
    ConfigToStringTestParam{
        .bufferSize = 32,
        .conversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 6}
        },
        .expectedBufferContent = u8"",
        .expectedConversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 6}
        }
    },
    ConfigToStringTestParam{
        .bufferSize = 33,
        .conversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 6}
        },
        .expectedBufferContent = u8",\"UintThird\":18446744073709551615",
        .expectedConversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 7}
        }
    },
    ConfigToStringTestParam{
        .bufferSize = 1,
        .conversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 7}
        },
        .expectedBufferContent = u8"}",
        .expectedConversionState{
            .nestingLevel = 0,
            .indexInNestingLevel{1}
        }
    }
)));

INSTANTIATE_TEST_SUITE_P(MultipleElementsInMultipleNestingLevels, ConfigToStringTest, testing::Combine(
testing::Values(
    [](ConfigToString& configToString) {
        configToString.beginObject(u8"Group1");
        configToString.beginObject(u8"Function1");
        configToString.boolean(u8"Enabled", [](bool){}, []{ return true; });
        configToString.endObject();
        configToString.beginObject(u8"Function2");
        configToString.boolean(u8"Enabled", [](bool){}, []{ return false; });
        configToString.uint(u8"Mode", [](std::uint64_t){}, []{ return 1024; });
        configToString.endObject();
        configToString.endObject();

        configToString.beginObject(u8"Group2");
        configToString.beginObject(u8"Function1");
        configToString.boolean(u8"Enabled", [](bool){}, []{ return false; });
        configToString.endObject();
        configToString.beginObject(u8"Function2");
        configToString.boolean(u8"Enabled", [](bool){}, []{ return true; });
        configToString.uint(u8"Mode", [](std::uint64_t){}, []{ return 514; });
        configToString.endObject();
        configToString.endObject();
    }
),
testing::Values(
    ConfigToStringTestParam{
        .bufferSize = 164,
        .expectedBufferContent = u8"{\"Group1\":{\"Function1\":{\"Enabled\":true},\"Function2\":{\"Enabled\":false,\"Mode\":1024}},\"Group2\":{\"Function1\":{\"Enabled\":false},\"Function2\":{\"Enabled\":true,\"Mode\":514}}}",
        .expectedConversionState{
            .nestingLevel = 0,
            .indexInNestingLevel{1}
        }
    },
    ConfigToStringTestParam{
        .bufferSize = 1000,
        .expectedBufferContent = u8"{\"Group1\":{\"Function1\":{\"Enabled\":true},\"Function2\":{\"Enabled\":false,\"Mode\":1024}},\"Group2\":{\"Function1\":{\"Enabled\":false},\"Function2\":{\"Enabled\":true,\"Mode\":514}}}",
        .expectedConversionState{
            .nestingLevel = 0,
            .indexInNestingLevel{1}
        }
    },
    ConfigToStringTestParam{
        .bufferSize = 1,
        .expectedBufferContent = u8"{",
        .expectedConversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 0}
        }
    },
    ConfigToStringTestParam{
        .bufferSize = 9,
        .conversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 0}
        },
        .expectedBufferContent = u8"",
        .expectedConversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 0}
        }
    },
    ConfigToStringTestParam{
        .bufferSize = 10,
        .conversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 0}
        },
        .expectedBufferContent = u8"\"Group1\":{",
        .expectedConversionState{
            .nestingLevel = 2,
            .indexInNestingLevel{0, 0, 0}
        }
    },
    ConfigToStringTestParam{
        .bufferSize = 12,
        .conversionState{
            .nestingLevel = 2,
            .indexInNestingLevel{0, 0, 0}
        },
        .expectedBufferContent = u8"",
        .expectedConversionState{
            .nestingLevel = 2,
            .indexInNestingLevel{0, 0, 0}
        }
    },
    ConfigToStringTestParam{
        .bufferSize = 13,
        .conversionState{
            .nestingLevel = 2,
            .indexInNestingLevel{0, 0, 0}
        },
        .expectedBufferContent = u8"\"Function1\":{",
        .expectedConversionState{
            .nestingLevel = 3,
            .indexInNestingLevel{0, 0, 0, 0}
        }
    },
    ConfigToStringTestParam{
        .bufferSize = 13,
        .conversionState{
            .nestingLevel = 3,
            .indexInNestingLevel{0, 0, 0, 0}
        },
        .expectedBufferContent = u8"",
        .expectedConversionState{
            .nestingLevel = 3,
            .indexInNestingLevel{0, 0, 0, 0}
        }
    },
    ConfigToStringTestParam{
        .bufferSize = 14,
        .conversionState{
            .nestingLevel = 3,
            .indexInNestingLevel{0, 0, 0, 0}
        },
        .expectedBufferContent = u8"\"Enabled\":true",
        .expectedConversionState{
            .nestingLevel = 3,
            .indexInNestingLevel{0, 0, 0, 1}
        }
    },
    ConfigToStringTestParam{
        .bufferSize = 14,
        .conversionState{
            .nestingLevel = 3,
            .indexInNestingLevel{0, 0, 0, 1}
        },
        .expectedBufferContent = u8"}",
        .expectedConversionState{
            .nestingLevel = 2,
            .indexInNestingLevel{0, 0, 1}
        }
    },
    ConfigToStringTestParam{
        .bufferSize = 14,
        .conversionState{
            .nestingLevel = 2,
            .indexInNestingLevel{0, 0, 1}
        },
        .expectedBufferContent = u8",\"Function2\":{",
        .expectedConversionState{
            .nestingLevel = 3,
            .indexInNestingLevel{0, 0, 1, 0}
        }
    },
    ConfigToStringTestParam{
        .bufferSize = 14,
        .conversionState{
            .nestingLevel = 3,
            .indexInNestingLevel{0, 0, 1, 0}
        },
        .expectedBufferContent = u8"",
        .expectedConversionState{
            .nestingLevel = 3,
            .indexInNestingLevel{0, 0, 1, 0}
        }
    },
    ConfigToStringTestParam{
        .bufferSize = 15,
        .conversionState{
            .nestingLevel = 3,
            .indexInNestingLevel{0, 0, 1, 0}
        },
        .expectedBufferContent = u8"\"Enabled\":false",
        .expectedConversionState{
            .nestingLevel = 3,
            .indexInNestingLevel{0, 0, 1, 1}
        }
    },
    ConfigToStringTestParam{
        .bufferSize = 11,
        .conversionState{
            .nestingLevel = 3,
            .indexInNestingLevel{0, 0, 1, 1}
        },
        .expectedBufferContent = u8"",
        .expectedConversionState{
            .nestingLevel = 3,
            .indexInNestingLevel{0, 0, 1, 1}
        }
    },
    ConfigToStringTestParam{
        .bufferSize = 12,
        .conversionState{
            .nestingLevel = 3,
            .indexInNestingLevel{0, 0, 1, 1}
        },
        .expectedBufferContent = u8",\"Mode\":1024",
        .expectedConversionState{
            .nestingLevel = 3,
            .indexInNestingLevel{0, 0, 1, 2}
        }
    },
    ConfigToStringTestParam{
        .bufferSize = 1,
        .conversionState{
            .nestingLevel = 3,
            .indexInNestingLevel{0, 0, 1, 2}
        },
        .expectedBufferContent = u8"}",
        .expectedConversionState{
            .nestingLevel = 2,
            .indexInNestingLevel{0, 0, 2}
        }
    },
    ConfigToStringTestParam{
        .bufferSize = 11,
        .conversionState{
            .nestingLevel = 2,
            .indexInNestingLevel{0, 0, 2}
        },
        .expectedBufferContent = u8"}",
        .expectedConversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 1}
        }
    },
    ConfigToStringTestParam{
        .bufferSize = 11,
        .conversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 1}
        },
        .expectedBufferContent = u8",\"Group2\":{",
        .expectedConversionState{
            .nestingLevel = 2,
            .indexInNestingLevel{0, 1, 0}
        }
    },
    ConfigToStringTestParam{
        .bufferSize = 12,
        .conversionState{
            .nestingLevel = 2,
            .indexInNestingLevel{0, 1, 0}
        },
        .expectedBufferContent = u8"",
        .expectedConversionState{
            .nestingLevel = 2,
            .indexInNestingLevel{0, 1, 0}
        }
    },
    ConfigToStringTestParam{
        .bufferSize = 13,
        .conversionState{
            .nestingLevel = 2,
            .indexInNestingLevel{0, 1, 0}
        },
        .expectedBufferContent = u8"\"Function1\":{",
        .expectedConversionState{
            .nestingLevel = 3,
            .indexInNestingLevel{0, 1, 0, 0}
        }
    },
    ConfigToStringTestParam{
        .bufferSize = 14,
        .conversionState{
            .nestingLevel = 3,
            .indexInNestingLevel{0, 1, 0, 0}
        },
        .expectedBufferContent = u8"",
        .expectedConversionState{
            .nestingLevel = 3,
            .indexInNestingLevel{0, 1, 0, 0}
        }
    },
    ConfigToStringTestParam{
        .bufferSize = 15,
        .conversionState{
            .nestingLevel = 3,
            .indexInNestingLevel{0, 1, 0, 0}
        },
        .expectedBufferContent = u8"\"Enabled\":false",
        .expectedConversionState{
            .nestingLevel = 3,
            .indexInNestingLevel{0, 1, 0, 1}
        }
    },
    ConfigToStringTestParam{
        .bufferSize = 14,
        .conversionState{
            .nestingLevel = 3,
            .indexInNestingLevel{0, 1, 0, 1}
        },
        .expectedBufferContent = u8"}",
        .expectedConversionState{
            .nestingLevel = 2,
            .indexInNestingLevel{0, 1, 1}
        }
    },
    ConfigToStringTestParam{
        .bufferSize = 14,
        .conversionState{
            .nestingLevel = 2,
            .indexInNestingLevel{0, 1, 1}
        },
        .expectedBufferContent = u8",\"Function2\":{",
        .expectedConversionState{
            .nestingLevel = 3,
            .indexInNestingLevel{0, 1, 1, 0}
        }
    },
    ConfigToStringTestParam{
        .bufferSize = 13,
        .conversionState{
            .nestingLevel = 3,
            .indexInNestingLevel{0, 1, 1, 0}
        },
        .expectedBufferContent = u8"",
        .expectedConversionState{
            .nestingLevel = 3,
            .indexInNestingLevel{0, 1, 1, 0}
        }
    },
    ConfigToStringTestParam{
        .bufferSize = 14,
        .conversionState{
            .nestingLevel = 3,
            .indexInNestingLevel{0, 1, 1, 0}
        },
        .expectedBufferContent = u8"\"Enabled\":true",
        .expectedConversionState{
            .nestingLevel = 3,
            .indexInNestingLevel{0, 1, 1, 1}
        }
    },
    ConfigToStringTestParam{
        .bufferSize = 10,
        .conversionState{
            .nestingLevel = 3,
            .indexInNestingLevel{0, 1, 1, 1}
        },
        .expectedBufferContent = u8"",
        .expectedConversionState{
            .nestingLevel = 3,
            .indexInNestingLevel{0, 1, 1, 1}
        }
    },
    ConfigToStringTestParam{
        .bufferSize = 11,
        .conversionState{
            .nestingLevel = 3,
            .indexInNestingLevel{0, 1, 1, 1}
        },
        .expectedBufferContent = u8",\"Mode\":514",
        .expectedConversionState{
            .nestingLevel = 3,
            .indexInNestingLevel{0, 1, 1, 2}
        }
    },
    ConfigToStringTestParam{
        .bufferSize = 1,
        .conversionState{
            .nestingLevel = 3,
            .indexInNestingLevel{0, 1, 1, 2}
        },
        .expectedBufferContent = u8"}",
        .expectedConversionState{
            .nestingLevel = 2,
            .indexInNestingLevel{0, 1, 2}
        }
    },
    ConfigToStringTestParam{
        .bufferSize = 1,
        .conversionState{
            .nestingLevel = 2,
            .indexInNestingLevel{0, 1, 2}
        },
        .expectedBufferContent = u8"}",
        .expectedConversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 2}
        }
    },
    ConfigToStringTestParam{
        .bufferSize = 1,
        .conversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 2}
        },
        .expectedBufferContent = u8"}",
        .expectedConversionState{
            .nestingLevel = 0,
            .indexInNestingLevel{1}
        }
    }
)));
