#include <cstdint>
#include <string>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <Config/ConfigStringConversionState.h>
#include <Config/ConfigFromString.h>

struct MockConfigValueSetter {
    MOCK_METHOD(void, setValueBool, (std::string id, bool value));
    MOCK_METHOD(void, setValueUint, (std::string id, std::uint64_t value));

    [[nodiscard]] auto boolSetterFunctor(std::string id)
    {
        return [this, id](bool value) {
            setValueBool(id, value);
        };
    }

    [[nodiscard]] auto uintSetterFunctor(std::string id)
    {
        return [this, id](std::uint64_t value) {
            setValueUint(id, value);
        };
    }
};

struct ConfigFromStringTestParam {
    std::u8string_view bufferContent{};
    ConfigStringConversionState conversionState{};
    std::function<void(MockConfigValueSetter&)> setExpectations = [](MockConfigValueSetter&){};
    ConfigStringConversionState expectedConversionState{};
    std::size_t expectedReadBytes{};
};

using CreateConfigFromStringSchemaFunctor = std::function<void(ConfigFromString&, MockConfigValueSetter&)>;

class ConfigFromStringTest : public testing::TestWithParam<std::tuple<CreateConfigFromStringSchemaFunctor, ConfigFromStringTestParam>> {
protected:
    testing::StrictMock<MockConfigValueSetter> mockConfigValueSetter;
};

TEST_P(ConfigFromStringTest, TestStringToConfigConversion) {
    const auto& param = std::get<1>(GetParam());

    param.setExpectations(mockConfigValueSetter);

    auto conversionState = param.conversionState;
    ConfigFromString configFromString{param.bufferContent, conversionState};
    configFromString.beginRoot();
    std::get<0>(GetParam())(configFromString, mockConfigValueSetter);
    const auto result = configFromString.endRoot();

    EXPECT_EQ(result, param.expectedReadBytes);
    EXPECT_EQ(conversionState.offset, param.expectedConversionState.offset);
    EXPECT_EQ(conversionState.nestingLevel, param.expectedConversionState.nestingLevel);
    for (auto nestingLevel = 0u; nestingLevel <= param.expectedConversionState.nestingLevel; ++nestingLevel)
        EXPECT_EQ(conversionState.indexInNestingLevel[nestingLevel], param.expectedConversionState.indexInNestingLevel[nestingLevel]);
}

INSTANTIATE_TEST_SUITE_P(EmptyRoot, ConfigFromStringTest, testing::Combine(
testing::Values([](ConfigFromString&, MockConfigValueSetter&) {}),
testing::Values(
    ConfigFromStringTestParam{
        .bufferContent = u8"{}",
        .expectedConversionState{
            .nestingLevel = 0,
            .indexInNestingLevel{1},
            .offset = 2
        },
        .expectedReadBytes = 2
    },
    ConfigFromStringTestParam{
        .bufferContent = u8"",
        .expectedConversionState{
            .nestingLevel = 0,
            .indexInNestingLevel{0},
            .offset = 0
        },
        .expectedReadBytes = 0
    },
    ConfigFromStringTestParam{
        .bufferContent = u8"{",
        .expectedConversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, config_params::kInvalidObjectIndex},
            .offset = 1
        },
        .expectedReadBytes = 1
    },
    ConfigFromStringTestParam{
        .bufferContent = u8"}",
        .conversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, config_params::kInvalidObjectIndex},
            .offset = 1
        },
        .expectedConversionState{
            .nestingLevel = 0,
            .indexInNestingLevel{1},
            .offset = 2
        },
        .expectedReadBytes = 1
    },
        ConfigFromStringTestParam{
        .bufferContent = u8" \n\t\r{\n\t\t\n}\n\n",
        .expectedConversionState{
            .nestingLevel = 0,
            .indexInNestingLevel{1},
            .offset = 10
        },
        .expectedReadBytes = 10
    },
    ConfigFromStringTestParam{
        .bufferContent = u8"\n \n\r\t{\n\t \r\t",
        .expectedConversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, config_params::kInvalidObjectIndex},
            .offset = 11
        },
        .expectedReadBytes = 11
    },
    ConfigFromStringTestParam{
        .bufferContent = u8"\r\t\n }\t\n\r ",
        .conversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, config_params::kInvalidObjectIndex},
            .offset = 11
        },
        .expectedConversionState{
            .nestingLevel = 0,
            .indexInNestingLevel{1},
            .offset = 16
        },
        .expectedReadBytes = 5
    },
    ConfigFromStringTestParam{
        .bufferContent = u8"{x",
        .expectedConversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, config_params::kInvalidObjectIndex},
            .offset = 1
        },
        .expectedReadBytes = 1
    },
    ConfigFromStringTestParam{
        .bufferContent = u8"x}",
        .conversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, config_params::kInvalidObjectIndex},
            .offset = 1
        },
        .expectedConversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, config_params::kInvalidObjectIndex},
            .offset = 1
        },
        .expectedReadBytes = 0
    },
    ConfigFromStringTestParam{
        .bufferContent = u8"}{",
        .expectedConversionState{
            .nestingLevel = 0,
            .indexInNestingLevel{0},
            .offset = 0
        },
        .expectedReadBytes = 0
    }
)));

INSTANTIATE_TEST_SUITE_P(OneObjectInRoot, ConfigFromStringTest, testing::Combine(
testing::Values([](ConfigFromString& configFromString, MockConfigValueSetter&) {
    configFromString.beginObject(u8"Obj1");
    configFromString.endObject();
}),
testing::Values(
    ConfigFromStringTestParam{
        .bufferContent = u8"{\"Obj1\":{}}",
        .expectedConversionState{
            .nestingLevel = 0,
            .indexInNestingLevel{1},
            .offset = 11
        },
        .expectedReadBytes = 11
    },
    ConfigFromStringTestParam{
        .bufferContent = u8"",
        .expectedConversionState{
            .nestingLevel = 0,
            .indexInNestingLevel{0},
            .offset = 0
        },
        .expectedReadBytes = 0
    },
    ConfigFromStringTestParam{
        .bufferContent = u8"{",
        .expectedConversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, config_params::kInvalidObjectIndex},
            .offset = 1
        },
        .expectedReadBytes = 1
    },
    ConfigFromStringTestParam{
        .bufferContent = u8"\"Obj1\":",
        .conversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, config_params::kInvalidObjectIndex},
            .offset = 1
        },
        .expectedConversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, config_params::kInvalidObjectIndex},
            .offset = 1
        },
        .expectedReadBytes = 0
    },
    ConfigFromStringTestParam{
        .bufferContent = u8"\"Obj1\":{",
        .conversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, config_params::kInvalidObjectIndex},
            .offset = 1
        },
        .expectedConversionState{
            .nestingLevel = 2,
            .indexInNestingLevel{0, 0, config_params::kInvalidObjectIndex},
            .offset = 9
        },
        .expectedReadBytes = 8
    },
    ConfigFromStringTestParam{
        .bufferContent = u8"}",
        .conversionState{
            .nestingLevel = 2,
            .indexInNestingLevel{0, 0, config_params::kInvalidObjectIndex},
            .offset = 9
        },
        .expectedConversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 0},
            .offset = 10
        },
        .expectedReadBytes = 1
    },
    ConfigFromStringTestParam{
        .bufferContent = u8"}",
        .conversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 0},
            .offset = 10
        },
        .expectedConversionState{
            .nestingLevel = 0,
            .indexInNestingLevel{1},
            .offset = 11
        },
        .expectedReadBytes = 1
    },
    ConfigFromStringTestParam{
        .bufferContent = u8"\n\r\r{\n \"Obj1\"\n\r\r:\n\n{\r\t}\t\n}\t ",
        .expectedConversionState{
            .nestingLevel = 0,
            .indexInNestingLevel{1},
            .offset = 25
        },
        .expectedReadBytes = 25
    },
    ConfigFromStringTestParam{
        .bufferContent = u8"\r\t \n{\r\n\n",
        .expectedConversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, config_params::kInvalidObjectIndex},
            .offset = 8
        },
        .expectedReadBytes = 8
    },
    ConfigFromStringTestParam{
        .bufferContent = u8" \n\n\t\"Obj1\"\n :\t",
        .conversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, config_params::kInvalidObjectIndex},
            .offset = 8
        },
        .expectedConversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, config_params::kInvalidObjectIndex},
            .offset = 12
        },
        .expectedReadBytes = 4
    },
    ConfigFromStringTestParam{
        .bufferContent = u8"\"Obj1\"\n :\t\n {\r\r",
        .conversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, config_params::kInvalidObjectIndex},
            .offset = 12
        },
        .expectedConversionState{
            .nestingLevel = 2,
            .indexInNestingLevel{0, 0, config_params::kInvalidObjectIndex},
            .offset = 27
        },
        .expectedReadBytes = 15
    },
    ConfigFromStringTestParam{
        .bufferContent = u8"\n\n}\r\r",
        .conversionState{
            .nestingLevel = 2,
            .indexInNestingLevel{0, 0, config_params::kInvalidObjectIndex},
            .offset = 27
        },
        .expectedConversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 0},
            .offset = 32
        },
        .expectedReadBytes = 5
    },
    ConfigFromStringTestParam{
        .bufferContent = u8"  } \n",
        .conversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 0},
            .offset = 32
        },
        .expectedConversionState{
            .nestingLevel = 0,
            .indexInNestingLevel{1},
            .offset = 35
        },
        .expectedReadBytes = 3
    },
    ConfigFromStringTestParam{
        .bufferContent = u8"{\"Obj12\":{}}",
        .expectedConversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, config_params::kInvalidObjectIndex},
            .offset = 1
        },
        .expectedReadBytes = 1
    },
    ConfigFromStringTestParam{
        .bufferContent = u8"{}",
        .expectedConversionState{
            .nestingLevel = 0,
            .indexInNestingLevel{1},
            .offset = 2
        },
        .expectedReadBytes = 2
    }
)));

INSTANTIATE_TEST_SUITE_P(MultipleElementsInRoot, ConfigFromStringTest, testing::Combine(
testing::Values([](ConfigFromString& configFromString, MockConfigValueSetter& mockConfigValueSetter) {
    configFromString.boolean(u8"BoolFirst", mockConfigValueSetter.boolSetterFunctor("BoolFirst"), []{ return bool{}; });
    configFromString.beginObject(u8"Obj1");
    configFromString.endObject();
    configFromString.uint(u8"UintFirst", mockConfigValueSetter.uintSetterFunctor("UintFirst"), []{ return std::uint64_t{}; });
    configFromString.boolean(u8"BoolSecond", mockConfigValueSetter.boolSetterFunctor("BoolSecond"), []{ return bool{}; });
    configFromString.beginObject(u8"Obj2");
    configFromString.endObject();
    configFromString.uint(u8"UintSecond", mockConfigValueSetter.uintSetterFunctor("UintSecond"), []{ return std::uint64_t{}; });
    configFromString.uint(u8"UintThird", mockConfigValueSetter.uintSetterFunctor("UintThird"), []{ return std::uint64_t{}; });
}),
testing::Values(
    ConfigFromStringTestParam{
        .bufferContent = u8"{\"BoolFirst\":false,\"Obj1\":{},\"UintFirst\":0,\"BoolSecond\":true,\"Obj2\":{},\"UintSecond\":123,\"UintThird\":18446744073709551615}",
        .setExpectations = [](MockConfigValueSetter& mockConfigValueSetter) {
            EXPECT_CALL(mockConfigValueSetter, setValueBool("BoolFirst", false));
            EXPECT_CALL(mockConfigValueSetter, setValueUint("UintFirst", 0));
            EXPECT_CALL(mockConfigValueSetter, setValueBool("BoolSecond", true));
            EXPECT_CALL(mockConfigValueSetter, setValueUint("UintSecond", 123));
            EXPECT_CALL(mockConfigValueSetter, setValueUint("UintThird", 18446744073709551615u));
        },
        .expectedConversionState{
            .nestingLevel = 0,
            .indexInNestingLevel{1},
            .offset = 121
        },
        .expectedReadBytes = 121
    },
    ConfigFromStringTestParam{
        .bufferContent = u8"",
        .expectedConversionState{
            .nestingLevel = 0,
            .indexInNestingLevel{0},
            .offset = 0
        },
        .expectedReadBytes = 0
    },
    ConfigFromStringTestParam{
        .bufferContent = u8"{\"BoolFirst\":fals",
        .expectedConversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, config_params::kInvalidObjectIndex},
            .offset = 1
        },
        .expectedReadBytes = 1
    },
    ConfigFromStringTestParam{
        .bufferContent = u8"\"BoolFirst\":false",
        .conversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, config_params::kInvalidObjectIndex},
            .offset = 1
        },
        .setExpectations = [](MockConfigValueSetter& mockConfigValueSetter) {
            EXPECT_CALL(mockConfigValueSetter, setValueBool("BoolFirst", false));
        },
        .expectedConversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 0},
            .offset = 18
        },
        .expectedReadBytes = 17
    },
    ConfigFromStringTestParam{
        .bufferContent = u8",\"Obj1\":",
        .conversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 0},
            .offset = 18
        },
        .expectedConversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 0},
            .offset = 18
        },
        .expectedReadBytes = 0
    },
    ConfigFromStringTestParam{
        .bufferContent = u8",\"Obj1\":{",
        .conversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 0},
            .offset = 18
        },
        .expectedConversionState{
            .nestingLevel = 2,
            .indexInNestingLevel{0, 1, config_params::kInvalidObjectIndex},
            .offset = 27
        },
        .expectedReadBytes = 9
    },
    ConfigFromStringTestParam{
        .bufferContent = u8"}",
        .conversionState{
            .nestingLevel = 2,
            .indexInNestingLevel{0, 1, config_params::kInvalidObjectIndex},
            .offset = 27
        },
        .expectedConversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 1},
            .offset = 28
        },
        .expectedReadBytes = 1
    },
    ConfigFromStringTestParam{
        .bufferContent = u8",\"UintFirst\":0",
        .conversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 1},
            .offset = 28
        },
        .expectedConversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 1},
            .offset = 28
        },
        .expectedReadBytes = 0
    },
    ConfigFromStringTestParam{
        .bufferContent = u8",\"UintFirst\":0,",
        .conversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 1},
            .offset = 28
        },
        .setExpectations = [](MockConfigValueSetter& mockConfigValueSetter) {
            EXPECT_CALL(mockConfigValueSetter, setValueUint("UintFirst", 0));
        },
        .expectedConversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 0},
            .offset = 42
        },
        .expectedReadBytes = 14
    },
    ConfigFromStringTestParam{
        .bufferContent = u8",\"BoolSecond\":tru",
        .conversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 0},
            .offset = 42
        },
        .expectedConversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 0},
            .offset = 42
        },
        .expectedReadBytes = 0
    },
    ConfigFromStringTestParam{
        .bufferContent = u8",\"BoolSecond\":true",
        .conversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 0},
            .offset = 42
        },
        .setExpectations = [](MockConfigValueSetter& mockConfigValueSetter) {
            EXPECT_CALL(mockConfigValueSetter, setValueBool("BoolSecond", true));
        },
        .expectedConversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 0},
            .offset = 60
        },
        .expectedReadBytes = 18
    },
    ConfigFromStringTestParam{
        .bufferContent = u8",\"Obj2\":",
        .conversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 0},
            .offset = 60
        },
        .expectedConversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 0},
            .offset = 60
        },
        .expectedReadBytes = 0
    },
    ConfigFromStringTestParam{
        .bufferContent = u8",\"Obj2\":{",
        .conversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 0},
            .offset = 60
        },
        .expectedConversionState{
            .nestingLevel = 2,
            .indexInNestingLevel{0, 4, config_params::kInvalidObjectIndex},
            .offset = 69
        },
        .expectedReadBytes = 9
    },
    ConfigFromStringTestParam{
        .bufferContent = u8"}",
        .conversionState{
            .nestingLevel = 2,
            .indexInNestingLevel{0, 4, config_params::kInvalidObjectIndex},
            .offset = 69
        },
        .expectedConversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 4},
            .offset = 70
        },
        .expectedReadBytes = 1
    },
    ConfigFromStringTestParam{
        .bufferContent = u8",\"UintSecond\":123",
        .conversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 4},
            .offset = 70
        },
        .expectedConversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 4},
            .offset = 70
        },
        .expectedReadBytes = 0
    },
    ConfigFromStringTestParam{
        .bufferContent = u8",\"UintSecond\":123,",
        .conversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 4},
            .offset = 70
        },
        .setExpectations = [](MockConfigValueSetter& mockConfigValueSetter) {
            EXPECT_CALL(mockConfigValueSetter, setValueUint("UintSecond", 123));
        },
        .expectedConversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 0},
            .offset = 87
        },
        .expectedReadBytes = 17
    },
    ConfigFromStringTestParam{
        .bufferContent = u8",\"UintThird\":18446744073709551615",
        .conversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 0},
            .offset = 87
        },
        .expectedConversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 0},
            .offset = 87
        },
        .expectedReadBytes = 0
    },
    ConfigFromStringTestParam{
        .bufferContent = u8",\"UintThird\":18446744073709551615}",
        .conversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 0},
            .offset = 87
        },
        .setExpectations = [](MockConfigValueSetter& mockConfigValueSetter) {
            EXPECT_CALL(mockConfigValueSetter, setValueUint("UintThird", 18446744073709551615u));
        },
        .expectedConversionState{
            .nestingLevel = 0,
            .indexInNestingLevel{1},
            .offset = 121
        },
        .expectedReadBytes = 34
    },
    ConfigFromStringTestParam{
        .bufferContent = u8"\n\n{\n\t\"BoolFirst\" \n\n:\t\r\n false\n\t, \"Obj1\" :\t{  } \r,  \t\"UintFirst\"\t:\t0\n,\r\"BoolSecond\"\r\r:\ntrue\n,\n\"Obj2\" : \n\n{  }\n,\r\"UintSecond\"\t:\t123\r,\n\"UintThird\" \r:\r\n18446744073709551615\n}\n",
        .setExpectations = [](MockConfigValueSetter& mockConfigValueSetter) {
            EXPECT_CALL(mockConfigValueSetter, setValueBool("BoolFirst", false));
            EXPECT_CALL(mockConfigValueSetter, setValueUint("UintFirst", 0));
            EXPECT_CALL(mockConfigValueSetter, setValueBool("BoolSecond", true));
            EXPECT_CALL(mockConfigValueSetter, setValueUint("UintSecond", 123));
            EXPECT_CALL(mockConfigValueSetter, setValueUint("UintThird", 18446744073709551615u));
        },
        .expectedConversionState{
            .nestingLevel = 0,
            .indexInNestingLevel{1},
            .offset = 170
        },
        .expectedReadBytes = 170
    },
    ConfigFromStringTestParam{
        .bufferContent = u8"\n\n{\n\t\"BoolFirst\" \n\n:\t\r\n fals",
        .expectedConversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, config_params::kInvalidObjectIndex},
            .offset = 5
        },
        .expectedReadBytes = 5
    },
    ConfigFromStringTestParam{
        .bufferContent = u8"\"BoolFirst\" \n\n:\t\r\n false\n\t, \"Obj1\" :\t",
        .conversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, config_params::kInvalidObjectIndex},
            .offset = 5
        },
        .setExpectations = [](MockConfigValueSetter& mockConfigValueSetter) {
            EXPECT_CALL(mockConfigValueSetter, setValueBool("BoolFirst", false));
        },
        .expectedConversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 0},
            .offset = 31
        },
        .expectedReadBytes = 26
    },
    ConfigFromStringTestParam{
        .bufferContent = u8", \"Obj1\" :\t{  ",
        .conversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 0},
            .offset = 31
        },
        .expectedConversionState{
            .nestingLevel = 2,
            .indexInNestingLevel{0, 1, config_params::kInvalidObjectIndex},
            .offset = 45
        },
        .expectedReadBytes = 14
    },
    ConfigFromStringTestParam{
        .bufferContent = u8"} \r,  \t\"UintFirst\"\t:\t0",
        .conversionState{
            .nestingLevel = 2,
            .indexInNestingLevel{0, 1, config_params::kInvalidObjectIndex},
            .offset = 45
        },
        .expectedConversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 1},
            .offset = 48
        },
        .expectedReadBytes = 3
    },
    ConfigFromStringTestParam{
        .bufferContent = u8",  \t\"UintFirst\"\t:\t0\n,\r\"BoolSecond\"\r\r:\ntru",
        .conversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 1},
            .offset = 48
        },
        .setExpectations = [](MockConfigValueSetter& mockConfigValueSetter) {
            EXPECT_CALL(mockConfigValueSetter, setValueUint("UintFirst", 0));
        },
        .expectedConversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 0},
            .offset = 68
        },
        .expectedReadBytes = 20
    },
    ConfigFromStringTestParam{
        .bufferContent = u8",\r\"BoolSecond\"\r\r:\ntrue",
        .conversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 0},
            .offset = 68
        },
        .setExpectations = [](MockConfigValueSetter& mockConfigValueSetter) {
            EXPECT_CALL(mockConfigValueSetter, setValueBool("BoolSecond", true));
        },
        .expectedConversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 0},
            .offset = 90
        },
        .expectedReadBytes = 22
    },
    ConfigFromStringTestParam{
        .bufferContent = u8"\n,\n\"Obj2\" : \n",
        .conversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 0},
            .offset = 90
        },
        .expectedConversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 0},
            .offset = 91
        },
        .expectedReadBytes = 1
    },
    ConfigFromStringTestParam{
        .bufferContent = u8",\n\"Obj2\" : \n\n{  ",
        .conversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 0},
            .offset = 91
        },
        .expectedConversionState{
            .nestingLevel = 2,
            .indexInNestingLevel{0, 4, config_params::kInvalidObjectIndex},
            .offset = 107
        },
        .expectedReadBytes = 16
    },
    ConfigFromStringTestParam{
        .bufferContent = u8"}\n,\r\"Uint",
        .conversionState{
            .nestingLevel = 2,
            .indexInNestingLevel{0, 4, config_params::kInvalidObjectIndex},
            .offset = 107
        },
        .expectedConversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 4},
            .offset = 109
        },
        .expectedReadBytes = 2
    },
    ConfigFromStringTestParam{
        .bufferContent = u8",\r\"UintSecond\"\t:\t123\r",
        .conversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 4},
            .offset = 109
        },
        .setExpectations = [](MockConfigValueSetter& mockConfigValueSetter) {
            EXPECT_CALL(mockConfigValueSetter, setValueUint("UintSecond", 123));
        },
        .expectedConversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 0},
            .offset = 130
        },
        .expectedReadBytes = 21
    },
    ConfigFromStringTestParam{
        .bufferContent = u8",\n\"UintThird\" \r:\r\n18446744073709551615",
        .conversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 0},
            .offset = 130
        },
        .expectedConversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 0},
            .offset = 130
        },
        .expectedReadBytes = 0
    },
    ConfigFromStringTestParam{
        .bufferContent = u8",\n\"UintThird\" \r:\r\n18446744073709551615\n",
        .conversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 0},
            .offset = 130
        },
        .setExpectations = [](MockConfigValueSetter& mockConfigValueSetter) {
            EXPECT_CALL(mockConfigValueSetter, setValueUint("UintThird", 18446744073709551615u));
        },
        .expectedConversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 0},
            .offset = 169
        },
        .expectedReadBytes = 39
    },
    ConfigFromStringTestParam{
        .bufferContent = u8"}\n",
        .conversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 0},
            .offset = 169
        },
        .expectedConversionState{
            .nestingLevel = 0,
            .indexInNestingLevel{1},
            .offset = 170
        },
        .expectedReadBytes = 1
    },
    ConfigFromStringTestParam{
        .bufferContent = u8"{\"UintThird\":18446744073709551615,\"UintSecond\":123,\"Obj2\":{},\"BoolSecond\":true,\"UintFirst\":0,\"Obj1\":{},\"BoolFirst\":false}",
        .setExpectations = [](MockConfigValueSetter& mockConfigValueSetter) {
            EXPECT_CALL(mockConfigValueSetter, setValueUint("UintThird", 18446744073709551615u));
        },
        .expectedConversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 0},
            .offset = 33
        },
        .expectedReadBytes = 33
    },
    ConfigFromStringTestParam{
        .bufferContent = u8",\"UintSecond\":123,\"Obj2\":{},\"BoolSecond\":true,\"UintFirst\":0,\"Obj1\":{},\"BoolFirst\":false}",
        .conversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 0},
            .offset = 33
        },
        .setExpectations = [](MockConfigValueSetter& mockConfigValueSetter) {
            EXPECT_CALL(mockConfigValueSetter, setValueUint("UintSecond", 123));
        },
        .expectedConversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 0},
            .offset = 50
        },
        .expectedReadBytes = 17
    },
    ConfigFromStringTestParam{
        .bufferContent = u8",\"Obj2\":{},\"BoolSecond\":true,\"UintFirst\":0,\"Obj1\":{},\"BoolFirst\":false}",
        .conversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 0},
            .offset = 50
        },
        .expectedConversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 4},
            .offset = 60
        },
        .expectedReadBytes = 10
    },
    ConfigFromStringTestParam{
        .bufferContent = u8",\"BoolSecond\":true,\"UintFirst\":0,\"Obj1\":{},\"BoolFirst\":false}",
        .conversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 4},
            .offset = 60
        },
        .setExpectations = [](MockConfigValueSetter& mockConfigValueSetter) {
            EXPECT_CALL(mockConfigValueSetter, setValueBool("BoolSecond", true));
        },
        .expectedConversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 0},
            .offset = 78
        },
        .expectedReadBytes = 18
    },
    ConfigFromStringTestParam{
        .bufferContent = u8",\"UintFirst\":0,\"Obj1\":{},\"BoolFirst\":false}",
        .conversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 0},
            .offset = 78
        },
        .setExpectations = [](MockConfigValueSetter& mockConfigValueSetter) {
            EXPECT_CALL(mockConfigValueSetter, setValueUint("UintFirst", 0));
        },
        .expectedConversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 0},
            .offset = 92
        },
        .expectedReadBytes = 14
    },
    ConfigFromStringTestParam{
        .bufferContent = u8",\"Obj1\":{},\"BoolFirst\":false}",
        .conversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 0},
            .offset = 92
        },
        .expectedConversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 1},
            .offset = 102
        },
        .expectedReadBytes = 10
    },
    ConfigFromStringTestParam{
        .bufferContent = u8",\"BoolFirst\":false}",
        .conversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 1},
            .offset = 102
        },
        .setExpectations = [](MockConfigValueSetter& mockConfigValueSetter) {
            EXPECT_CALL(mockConfigValueSetter, setValueBool("BoolFirst", false));
        },
        .expectedConversionState{
            .nestingLevel = 0,
            .indexInNestingLevel{1},
            .offset = 121
        },
        .expectedReadBytes = 19
    },
    ConfigFromStringTestParam{
        .bufferContent = u8"\r\n{\t \"UintThird\"\n:\r18446744073709551615\t,\t\"UintSecond\"\n:\t123\n,\r\"Obj2\" :\n{\r}\t,\n\"BoolSecond\" : true, \"UintFirst\":\n0\t, \"Obj1\":\t{\n\n},\n\"BoolFirst\": false }\r",
        .setExpectations = [](MockConfigValueSetter& mockConfigValueSetter) {
            EXPECT_CALL(mockConfigValueSetter, setValueUint("UintThird", 18446744073709551615u));
        },
        .expectedConversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 0},
            .offset = 40
        },
        .expectedReadBytes = 40
    },
    ConfigFromStringTestParam{
        .bufferContent = u8",\t\"UintSecond\"\n:\t123\n,\r\"Obj2\" :\n{\r}\t,\n\"BoolSecond\" : true, \"UintFirst\":\n0\t, \"Obj1\":\t{\n\n},\n\"BoolFirst\": false }\r",
        .conversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 0},
            .offset = 40
        },
        .setExpectations = [](MockConfigValueSetter& mockConfigValueSetter) {
            EXPECT_CALL(mockConfigValueSetter, setValueUint("UintSecond", 123));
        },
        .expectedConversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 0},
            .offset = 61
        },
        .expectedReadBytes = 21
    },
    ConfigFromStringTestParam{
        .bufferContent = u8",\r\"Obj2\" :\n{\r}\t,\n\"BoolSecond\" : true, \"UintFirst\":\n0\t, \"Obj1\":\t{\n\n},\n\"BoolFirst\": false }\r",
        .conversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 0},
            .offset = 61
        },
        .expectedConversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 4},
            .offset = 76
        },
        .expectedReadBytes = 15
    },
    ConfigFromStringTestParam{
        .bufferContent = u8",\n\"BoolSecond\" : true, \"UintFirst\":\n0\t, \"Obj1\":\t{\n\n},\n\"BoolFirst\": false }\r",
        .conversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 4},
            .offset = 76
        },
        .setExpectations = [](MockConfigValueSetter& mockConfigValueSetter) {
            EXPECT_CALL(mockConfigValueSetter, setValueBool("BoolSecond", true));
        },
        .expectedConversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 0},
            .offset = 97
        },
        .expectedReadBytes = 21
    },
    ConfigFromStringTestParam{
        .bufferContent = u8", \"UintFirst\":\n0\t, \"Obj1\":\t{\n\n},\n\"BoolFirst\": false }\r",
        .conversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 0},
            .offset = 97
        },
        .setExpectations = [](MockConfigValueSetter& mockConfigValueSetter) {
            EXPECT_CALL(mockConfigValueSetter, setValueUint("UintFirst", 0));
        },
        .expectedConversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 0},
            .offset = 114
        },
        .expectedReadBytes = 17
    },
    ConfigFromStringTestParam{
        .bufferContent = u8", \"Obj1\":\t{\n\n},\n\"BoolFirst\": false }\r",
        .conversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 0},
            .offset = 97
        },
        .expectedConversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 1},
            .offset = 111
        },
        .expectedReadBytes = 14
    },
    ConfigFromStringTestParam{
        .bufferContent = u8",\n\"BoolFirst\": false }\r",
        .conversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 1},
            .offset = 111
        },
        .setExpectations = [](MockConfigValueSetter& mockConfigValueSetter) {
            EXPECT_CALL(mockConfigValueSetter, setValueBool("BoolFirst", false));
        },
        .expectedConversionState{
            .nestingLevel = 0,
            .indexInNestingLevel{1},
            .offset = 133
        },
        .expectedReadBytes = 22
    },
    ConfigFromStringTestParam{
        .bufferContent = u8"{\"BoolFirst\":false}",
        .setExpectations = [](MockConfigValueSetter& mockConfigValueSetter) {
            EXPECT_CALL(mockConfigValueSetter, setValueBool("BoolFirst", false));
        },
        .expectedConversionState{
            .nestingLevel = 0,
            .indexInNestingLevel{1},
            .offset = 19
        },
        .expectedReadBytes = 19
    },
    ConfigFromStringTestParam{
        .bufferContent = u8"{\" BoolFirst\":false}",
        .expectedConversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, config_params::kInvalidObjectIndex},
            .offset = 1
        },
        .expectedReadBytes = 1
    },
    ConfigFromStringTestParam{
        .bufferContent = u8"{\"BoolFirst \":false}",
        .expectedConversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, config_params::kInvalidObjectIndex},
            .offset = 1
        },
        .expectedReadBytes = 1
    },
    ConfigFromStringTestParam{
        .bufferContent = u8"{\" BoolFirst \":false}",
        .expectedConversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, config_params::kInvalidObjectIndex},
            .offset = 1
        },
        .expectedReadBytes = 1
    },
    ConfigFromStringTestParam{
        .bufferContent = u8"{\"\rBoolFirst\n\":false}",
        .expectedConversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, config_params::kInvalidObjectIndex},
            .offset = 1
        },
        .expectedReadBytes = 1
    },
    ConfigFromStringTestParam{
        .bufferContent = u8"{\"\tBoolFirst\":false}",
        .expectedConversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, config_params::kInvalidObjectIndex},
            .offset = 1
        },
        .expectedReadBytes = 1
    },
    ConfigFromStringTestParam{
        .bufferContent = u8"{\"Bool First\":false}",
        .expectedConversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, config_params::kInvalidObjectIndex},
            .offset = 1
        },
        .expectedReadBytes = 1
    },
    ConfigFromStringTestParam{
        .bufferContent = u8"{\"BoolFirst\":false+}",
        .setExpectations = [](MockConfigValueSetter& mockConfigValueSetter) {
            EXPECT_CALL(mockConfigValueSetter, setValueBool("BoolFirst", false));
        },
        .expectedConversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 0},
            .offset = 18
        },
        .expectedReadBytes = 18
    },
    ConfigFromStringTestParam{
        .bufferContent = u8"{\"UintSecond\":123}",
        .setExpectations = [](MockConfigValueSetter& mockConfigValueSetter) {
            EXPECT_CALL(mockConfigValueSetter, setValueUint("UintSecond", 123));
        },
        .expectedConversionState{
            .nestingLevel = 0,
            .indexInNestingLevel{1},
            .offset = 18
        },
        .expectedReadBytes = 18
    },
    ConfigFromStringTestParam{
        .bufferContent = u8"{\"\tUintSecond\":123}",
        .expectedConversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, config_params::kInvalidObjectIndex},
            .offset = 1
        },
        .expectedReadBytes = 1
    },
    ConfigFromStringTestParam{
        .bufferContent = u8"{\"UintSecond\t\":123}",
        .expectedConversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, config_params::kInvalidObjectIndex},
            .offset = 1
        },
        .expectedReadBytes = 1
    },
    ConfigFromStringTestParam{
        .bufferContent = u8"{\"\tUintSecond\t\":123}",
        .expectedConversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, config_params::kInvalidObjectIndex},
            .offset = 1
        },
        .expectedReadBytes = 1
    },
    ConfigFromStringTestParam{
        .bufferContent = u8"{\"Uint Second\":123}",
        .expectedConversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, config_params::kInvalidObjectIndex},
            .offset = 1
        },
        .expectedReadBytes = 1
    },
    ConfigFromStringTestParam{
        .bufferContent = u8"{\"Obj1\":{}}",
        .expectedConversionState{
            .nestingLevel = 0,
            .indexInNestingLevel{1},
            .offset = 11
        },
        .expectedReadBytes = 11
    },
    ConfigFromStringTestParam{
        .bufferContent = u8"{\" Obj1\":{}}",
        .expectedConversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, config_params::kInvalidObjectIndex},
            .offset = 1
        },
        .expectedReadBytes = 1
    },
    ConfigFromStringTestParam{
        .bufferContent = u8"{\"Obj1 \":{}}",
        .expectedConversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, config_params::kInvalidObjectIndex},
            .offset = 1
        },
        .expectedReadBytes = 1
    },
    ConfigFromStringTestParam{
        .bufferContent = u8"{\" Obj1 \":{}}",
        .expectedConversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, config_params::kInvalidObjectIndex},
            .offset = 1
        },
        .expectedReadBytes = 1
    },
    ConfigFromStringTestParam{
        .bufferContent = u8"{\"Ob j1\":{}}",
        .expectedConversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, config_params::kInvalidObjectIndex},
            .offset = 1
        },
        .expectedReadBytes = 1
    },
    ConfigFromStringTestParam{
        .bufferContent = u8"{\"UintThird\":18446744073709551615}",
        .setExpectations = [](MockConfigValueSetter& mockConfigValueSetter) {
            EXPECT_CALL(mockConfigValueSetter, setValueUint("UintThird", 18446744073709551615u));
        },
        .expectedConversionState{
            .nestingLevel = 0,
            .indexInNestingLevel{1},
            .offset = 34
        },
        .expectedReadBytes = 34
    },
    ConfigFromStringTestParam{
        .bufferContent = u8"{\"UintThird\":184467440737095516155}",
        .expectedConversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, config_params::kInvalidObjectIndex},
            .offset = 1
        },
        .expectedReadBytes = 1
    }
)));

INSTANTIATE_TEST_SUITE_P(MultipleElementsInMultipleNestingLevels, ConfigFromStringTest, testing::Combine(
testing::Values([](ConfigFromString& configFromString, MockConfigValueSetter& mockConfigValueSetter) {
    configFromString.beginObject(u8"Group1");
    configFromString.beginObject(u8"Function1");
    configFromString.boolean(u8"Enabled", mockConfigValueSetter.boolSetterFunctor("Group1.Function1.Enabled"), []{ return bool{}; });
    configFromString.endObject();
    configFromString.beginObject(u8"Function2");
    configFromString.boolean(u8"Enabled", mockConfigValueSetter.boolSetterFunctor("Group1.Function2.Enabled"), []{ return bool{}; });
    configFromString.uint(u8"Mode", mockConfigValueSetter.uintSetterFunctor("Group1.Function2.Mode"), []{ return std::uint64_t{}; });
    configFromString.endObject();
    configFromString.endObject();

    configFromString.beginObject(u8"Group2");
    configFromString.beginObject(u8"Function1");
    configFromString.boolean(u8"Enabled", mockConfigValueSetter.boolSetterFunctor("Group2.Function1.Enabled"), []{ return bool{}; });
    configFromString.endObject();
    configFromString.beginObject(u8"Function2");
    configFromString.boolean(u8"Enabled", mockConfigValueSetter.boolSetterFunctor("Group2.Function2.Enabled"), []{ return bool{}; });
    configFromString.uint(u8"Mode", mockConfigValueSetter.uintSetterFunctor("Group2.Function2.Mode"), []{ return std::uint64_t{}; });
    configFromString.endObject();
    configFromString.endObject();
}),
testing::Values(
    ConfigFromStringTestParam{
        .bufferContent = u8"{\"Group1\":{\"Function1\":{\"Enabled\":true},\"Function2\":{\"Enabled\":false,\"Mode\":1024}},\"Group2\":{\"Function1\":{\"Enabled\":false},\"Function2\":{\"Enabled\":true,\"Mode\":514}}}",
        .setExpectations = [](MockConfigValueSetter& mockConfigValueSetter) {
            EXPECT_CALL(mockConfigValueSetter, setValueBool("Group1.Function1.Enabled", true));
            EXPECT_CALL(mockConfigValueSetter, setValueBool("Group1.Function2.Enabled", false));
            EXPECT_CALL(mockConfigValueSetter, setValueUint("Group1.Function2.Mode", 1024));
            EXPECT_CALL(mockConfigValueSetter, setValueBool("Group2.Function1.Enabled", false));
            EXPECT_CALL(mockConfigValueSetter, setValueBool("Group2.Function2.Enabled", true));
            EXPECT_CALL(mockConfigValueSetter, setValueUint("Group2.Function2.Mode", 514));
        },
        .expectedConversionState{
            .nestingLevel = 0,
            .indexInNestingLevel{1},
            .offset = 164
        },
        .expectedReadBytes = 164
    },
    ConfigFromStringTestParam{
        .bufferContent = u8"",
        .expectedConversionState{
            .nestingLevel = 0,
            .indexInNestingLevel{0},
            .offset = 0
        },
        .expectedReadBytes = 0
    },
    ConfigFromStringTestParam{
        .bufferContent = u8"{\"Group1\":",
        .expectedConversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, config_params::kInvalidObjectIndex},
            .offset = 1
        },
        .expectedReadBytes = 1
    },
    ConfigFromStringTestParam{
        .bufferContent = u8"\"Group1\":{\"Function1\":",
        .conversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, config_params::kInvalidObjectIndex},
            .offset = 1
        },
        .expectedConversionState{
            .nestingLevel = 2,
            .indexInNestingLevel{0, 0, config_params::kInvalidObjectIndex},
            .offset = 11
        },
        .expectedReadBytes = 10
    },
    ConfigFromStringTestParam{
        .bufferContent = u8"\"Function1\":{\"Enabled\":tru",
        .conversionState{
            .nestingLevel = 2,
            .indexInNestingLevel{0, 0, config_params::kInvalidObjectIndex},
            .offset = 11
        },
        .expectedConversionState{
            .nestingLevel = 3,
            .indexInNestingLevel{0, 0, 0, config_params::kInvalidObjectIndex},
            .offset = 24
        },
        .expectedReadBytes = 13
    },
    ConfigFromStringTestParam{
        .bufferContent = u8"\"Enabled\":true",
        .conversionState{
            .nestingLevel = 3,
            .indexInNestingLevel{0, 0, 0, config_params::kInvalidObjectIndex},
            .offset = 24
        },
        .setExpectations = [](MockConfigValueSetter& mockConfigValueSetter) {
            EXPECT_CALL(mockConfigValueSetter, setValueBool("Group1.Function1.Enabled", true));
        },
        .expectedConversionState{
            .nestingLevel = 3,
            .indexInNestingLevel{0, 0, 0, 0},
            .offset = 38
        },
        .expectedReadBytes = 14
    },
    ConfigFromStringTestParam{
        .bufferContent = u8"},\"Function2\":",
        .conversionState{
            .nestingLevel = 3,
            .indexInNestingLevel{0, 0, 0, 0},
            .offset = 38
        },
        .expectedConversionState{
            .nestingLevel = 2,
            .indexInNestingLevel{0, 0, 0},
            .offset = 39
        },
        .expectedReadBytes = 1
    },
    ConfigFromStringTestParam{
        .bufferContent = u8",\"Function2\":{\"Enabled\":fals",
        .conversionState{
            .nestingLevel = 2,
            .indexInNestingLevel{0, 0, 0},
            .offset = 39
        },
        .expectedConversionState{
            .nestingLevel = 3,
            .indexInNestingLevel{0, 0, 1, config_params::kInvalidObjectIndex},
            .offset = 53
        },
        .expectedReadBytes = 14
    },
    ConfigFromStringTestParam{
        .bufferContent = u8"\"Enabled\":false,\"Mode\":1024",
        .conversionState{
            .nestingLevel = 3,
            .indexInNestingLevel{0, 0, 1, config_params::kInvalidObjectIndex},
            .offset = 53
        },
        .setExpectations = [](MockConfigValueSetter& mockConfigValueSetter) {
            EXPECT_CALL(mockConfigValueSetter, setValueBool("Group1.Function2.Enabled", false));
        },
        .expectedConversionState{
            .nestingLevel = 3,
            .indexInNestingLevel{0, 0, 1, 0},
            .offset = 68
        },
        .expectedReadBytes = 15
    },
    ConfigFromStringTestParam{
        .bufferContent = u8",\"Mode\":1024}",
        .conversionState{
            .nestingLevel = 3,
            .indexInNestingLevel{0, 0, 1, 0},
            .offset = 68
        },
        .setExpectations = [](MockConfigValueSetter& mockConfigValueSetter) {
            EXPECT_CALL(mockConfigValueSetter, setValueUint("Group1.Function2.Mode", 1024));
        },
        .expectedConversionState{
            .nestingLevel = 2,
            .indexInNestingLevel{0, 0, 1},
            .offset = 81
        },
        .expectedReadBytes = 13
    },
    ConfigFromStringTestParam{
        .bufferContent = u8"},\"Group2\":",
        .conversionState{
            .nestingLevel = 2,
            .indexInNestingLevel{0, 0, 1},
            .offset = 81
        },
        .expectedConversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 0},
            .offset = 82
        },
        .expectedReadBytes = 1
    },
    ConfigFromStringTestParam{
        .bufferContent = u8",\"Group2\":{\"Function1\":",
        .conversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 0},
            .offset = 82
        },
        .expectedConversionState{
            .nestingLevel = 2,
            .indexInNestingLevel{0, 1, config_params::kInvalidObjectIndex},
            .offset = 93
        },
        .expectedReadBytes = 11
    },
    ConfigFromStringTestParam{
        .bufferContent = u8"\"Function1\":{\"Enabled\":fals",
        .conversionState{
            .nestingLevel = 2,
            .indexInNestingLevel{0, 1, config_params::kInvalidObjectIndex},
            .offset = 93
        },
        .expectedConversionState{
            .nestingLevel = 3,
            .indexInNestingLevel{0, 1, 0, config_params::kInvalidObjectIndex},
            .offset = 106
        },
        .expectedReadBytes = 13
    },
    ConfigFromStringTestParam{
        .bufferContent = u8"\"Enabled\":false",
        .conversionState{
            .nestingLevel = 3,
            .indexInNestingLevel{0, 1, 0, config_params::kInvalidObjectIndex},
            .offset = 106
        },
        .setExpectations = [](MockConfigValueSetter& mockConfigValueSetter) {
            EXPECT_CALL(mockConfigValueSetter, setValueBool("Group2.Function1.Enabled", false));
        },
        .expectedConversionState{
            .nestingLevel = 3,
            .indexInNestingLevel{0, 1, 0, 0},
            .offset = 121
        },
        .expectedReadBytes = 15
    },
    ConfigFromStringTestParam{
        .bufferContent = u8"}",
        .conversionState{
            .nestingLevel = 3,
            .indexInNestingLevel{0, 1, 0, 0},
            .offset = 121
        },
        .expectedConversionState{
            .nestingLevel = 2,
            .indexInNestingLevel{0, 1, 0},
            .offset = 122
        },
        .expectedReadBytes = 1
    },
    ConfigFromStringTestParam{
        .bufferContent = u8",\"Function2\":{",
        .conversionState{
            .nestingLevel = 2,
            .indexInNestingLevel{0, 1, 0},
            .offset = 122
        },
        .expectedConversionState{
            .nestingLevel = 3,
            .indexInNestingLevel{0, 1, 1, config_params::kInvalidObjectIndex},
            .offset = 136
        },
        .expectedReadBytes = 14
    },
    ConfigFromStringTestParam{
        .bufferContent = u8"\"Enabled\":true,",
        .conversionState{
            .nestingLevel = 3,
            .indexInNestingLevel{0, 1, 1, config_params::kInvalidObjectIndex},
            .offset = 136
        },
        .setExpectations = [](MockConfigValueSetter& mockConfigValueSetter) {
            EXPECT_CALL(mockConfigValueSetter, setValueBool("Group2.Function2.Enabled", true));
        },
        .expectedConversionState{
            .nestingLevel = 3,
            .indexInNestingLevel{0, 1, 1, 0},
            .offset = 150
        },
        .expectedReadBytes = 14
    },
    ConfigFromStringTestParam{
        .bufferContent = u8",\"Mode\":514}",
        .conversionState{
            .nestingLevel = 3,
            .indexInNestingLevel{0, 1, 1, 0},
            .offset = 150
        },
        .setExpectations = [](MockConfigValueSetter& mockConfigValueSetter) {
            EXPECT_CALL(mockConfigValueSetter, setValueUint("Group2.Function2.Mode", 514));
        },
        .expectedConversionState{
            .nestingLevel = 2,
            .indexInNestingLevel{0, 1, 1},
            .offset = 162
        },
        .expectedReadBytes = 12
    },
    ConfigFromStringTestParam{
        .bufferContent = u8"}}",
        .conversionState{
            .nestingLevel = 2,
            .indexInNestingLevel{0, 1, 1},
            .offset = 162
        },
        .expectedConversionState{
            .nestingLevel = 0,
            .indexInNestingLevel{1},
            .offset = 164
        },
        .expectedReadBytes = 2
    },
    ConfigFromStringTestParam{
        .bufferContent = u8"\n{\n\t\"Group1\"\n\r:\r { \"Function1\":\n{\n\"Enabled\":\ttrue\r}\n\n,\r\"Function2\": { \"Enabled\": false,\n\"Mode\":1024\n}\t\t}\r,\n\"Group2\":\t{\n\"Function1\" :{ \"Enabled\"\r:\nfalse\n},\"Function2\":{\"Enabled\":true\r\n,\r\n\"Mode\":514\n}}}",
        .setExpectations = [](MockConfigValueSetter& mockConfigValueSetter) {
            EXPECT_CALL(mockConfigValueSetter, setValueBool("Group1.Function1.Enabled", true));
            EXPECT_CALL(mockConfigValueSetter, setValueBool("Group1.Function2.Enabled", false));
            EXPECT_CALL(mockConfigValueSetter, setValueUint("Group1.Function2.Mode", 1024));
            EXPECT_CALL(mockConfigValueSetter, setValueBool("Group2.Function1.Enabled", false));
            EXPECT_CALL(mockConfigValueSetter, setValueBool("Group2.Function2.Enabled", true));
            EXPECT_CALL(mockConfigValueSetter, setValueUint("Group2.Function2.Mode", 514));
        },
        .expectedConversionState{
            .nestingLevel = 0,
            .indexInNestingLevel{1},
            .offset = 200
        },
        .expectedReadBytes = 200
    },
    ConfigFromStringTestParam{
        .bufferContent = u8"\n{\n",
        .expectedConversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, config_params::kInvalidObjectIndex},
            .offset = 3
        },
        .expectedReadBytes = 3
    },
    ConfigFromStringTestParam{
        .bufferContent = u8"\t\"Group1\"\n\r:\r {",
        .conversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, config_params::kInvalidObjectIndex},
            .offset = 3
        },
        .expectedConversionState{
            .nestingLevel = 2,
            .indexInNestingLevel{0, 0, config_params::kInvalidObjectIndex},
            .offset = 18
        },
        .expectedReadBytes = 15
    },
    ConfigFromStringTestParam{
        .bufferContent = u8" \"Function1\":\n{\n\"Enabled\":\ttrue\r",
        .conversionState{
            .nestingLevel = 2,
            .indexInNestingLevel{0, 0, config_params::kInvalidObjectIndex},
            .offset = 18
        },
        .setExpectations = [](MockConfigValueSetter& mockConfigValueSetter) {
            EXPECT_CALL(mockConfigValueSetter, setValueBool("Group1.Function1.Enabled", true));
        },
        .expectedConversionState{
            .nestingLevel = 3,
            .indexInNestingLevel{0, 0, 0, 0},
            .offset = 50
        },
        .expectedReadBytes = 32
    },
    ConfigFromStringTestParam{
        .bufferContent = u8"}\n\n,\r\"Function2\": { \"Enabled\": false",
        .conversionState{
            .nestingLevel = 3,
            .indexInNestingLevel{0, 0, 0, 0},
            .offset = 50
        },
        .setExpectations = [](MockConfigValueSetter& mockConfigValueSetter) {
            EXPECT_CALL(mockConfigValueSetter, setValueBool("Group1.Function2.Enabled", false));
        },
        .expectedConversionState{
            .nestingLevel = 3,
            .indexInNestingLevel{0, 0, 1, 0},
            .offset = 86
        },
        .expectedReadBytes = 36
    },
    ConfigFromStringTestParam{
        .bufferContent = u8",\n\"Mode\":1024\n}\t\t}\r,",
        .conversionState{
            .nestingLevel = 3,
            .indexInNestingLevel{0, 0, 1, 0},
            .offset = 86
        },
        .setExpectations = [](MockConfigValueSetter& mockConfigValueSetter) {
            EXPECT_CALL(mockConfigValueSetter, setValueUint("Group1.Function2.Mode", 1024));
        },
        .expectedConversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 0},
            .offset = 105
        },
        .expectedReadBytes = 19
    },
    ConfigFromStringTestParam{
        .bufferContent = u8",\n\"Group2\":\t{\n\"Function1\" :{ \"Enabled\"\r:\nfalse\n",
        .conversionState{
            .nestingLevel = 1,
            .indexInNestingLevel{0, 0},
            .offset = 105
        },
        .setExpectations = [](MockConfigValueSetter& mockConfigValueSetter) {
            EXPECT_CALL(mockConfigValueSetter, setValueBool("Group2.Function1.Enabled", false));
        },
        .expectedConversionState{
            .nestingLevel = 3,
            .indexInNestingLevel{0, 1, 0, 0},
            .offset = 152
        },
        .expectedReadBytes = 47
    },
    ConfigFromStringTestParam{
        .bufferContent = u8"},\"Function2\":{\"Enabled\":true\r\n",
        .conversionState{
            .nestingLevel = 3,
            .indexInNestingLevel{0, 1, 0, 0},
            .offset = 152
        },
        .setExpectations = [](MockConfigValueSetter& mockConfigValueSetter) {
            EXPECT_CALL(mockConfigValueSetter, setValueBool("Group2.Function2.Enabled", true));
        },
        .expectedConversionState{
            .nestingLevel = 3,
            .indexInNestingLevel{0, 1, 1, 0},
            .offset = 183
        },
        .expectedReadBytes = 31
    },
    ConfigFromStringTestParam{
        .bufferContent = u8",\r\n\"Mode\":514\n}}}",
        .conversionState{
            .nestingLevel = 3,
            .indexInNestingLevel{0, 1, 1, 0},
            .offset = 183
        },
        .setExpectations = [](MockConfigValueSetter& mockConfigValueSetter) {
            EXPECT_CALL(mockConfigValueSetter, setValueUint("Group2.Function2.Mode", 514));
        },
        .expectedConversionState{
            .nestingLevel = 0,
            .indexInNestingLevel{1},
            .offset = 200
        },
        .expectedReadBytes = 17
    }
)));
