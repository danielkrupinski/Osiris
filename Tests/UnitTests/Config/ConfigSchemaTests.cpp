#include <cstddef>
#include <set>
#include <vector>

#include <gtest/gtest.h>

#include <Config/ConfigParams.h>
#include <Config/ConfigSchema.h>
#include <Config/ConfigVariableTypes.h>

#include <Mocks/MockConfig.h>
#include <Mocks/MockConfigConversion.h>
#include <Mocks/MockHookContext.h>

class ConfigSchemaTest : public testing::Test {
protected:
    testing::StrictMock<MockHookContext> mockHookContext;
    testing::StrictMock<MockConfigConversion> mockConfigConversion;
    testing::StrictMock<MockConfig> mockConfig;

    ConfigSchema<MockHookContext> configSchema{mockHookContext};
    using IndexInNestingLevel = std::size_t;
    std::vector<IndexInNestingLevel> nestingLevels;
    std::set<std::size_t> configVariableIndexes;
};

TEST_F(ConfigSchemaTest, SchemaIsValid) {
    EXPECT_CALL(mockConfigConversion, beginRoot()).WillOnce(testing::Invoke([this] {
        EXPECT_EQ(nestingLevels.size(), 0);
        EXPECT_LE(nestingLevels.size(), config_params::kMaxNestingLevel);
        nestingLevels.push_back(IndexInNestingLevel{});
    }));

    EXPECT_CALL(mockConfigConversion, endRoot()).WillOnce(testing::Invoke([this] {
        EXPECT_EQ(nestingLevels.size(), 1);
        nestingLevels.clear();
    }));

    EXPECT_CALL(mockConfigConversion, beginObject(testing::_)).WillRepeatedly(testing::Invoke([this] {
        EXPECT_GT(nestingLevels.size(), 0);
        EXPECT_LE(nestingLevels.size(), config_params::kMaxNestingLevel);
        nestingLevels.push_back(IndexInNestingLevel{});
    }));

    EXPECT_CALL(mockConfigConversion, endObject()).WillRepeatedly(testing::Invoke([this] {
        EXPECT_GT(nestingLevels.size(), 1);
        nestingLevels.pop_back();
        EXPECT_LT(nestingLevels.back(), config_params::kMaxObjectIndex);
        ++nestingLevels.back();
    }));

    EXPECT_CALL(mockConfigConversion, boolean(testing::_, testing::_, testing::_)).WillRepeatedly(testing::Invoke([this] {
        EXPECT_GT(nestingLevels.size(), 0);
        EXPECT_LT(nestingLevels.back(), config_params::kMaxObjectIndex);
        ++nestingLevels.back();
    }));

    EXPECT_CALL(mockConfigConversion, uint(testing::_, testing::_, testing::_)).WillRepeatedly(testing::Invoke([this] {
        EXPECT_GT(nestingLevels.size(), 0);
        EXPECT_LT(nestingLevels.back(), config_params::kMaxObjectIndex);
        ++nestingLevels.back();
    }));

    configSchema.performConversion(mockConfigConversion);
}

TEST_F(ConfigSchemaTest, EachConfigVariableIsLoadedOnce) {
    EXPECT_CALL(mockConfigConversion, beginRoot());
    EXPECT_CALL(mockConfigConversion, endRoot());
    EXPECT_CALL(mockConfigConversion, beginObject(testing::_)).Times(testing::AnyNumber());
    EXPECT_CALL(mockConfigConversion, endObject()).Times(testing::AnyNumber());

    EXPECT_CALL(mockHookContext, config()).WillRepeatedly(testing::ReturnRef(mockConfig));

    EXPECT_CALL(mockConfigConversion, boolean(testing::_, testing::_, testing::_))
        .WillRepeatedly(testing::WithArg<1>(testing::Invoke([this](auto valueSetter) {
            EXPECT_CALL(mockConfig, setVariableWithoutAutoSave(testing::_, testing::_))
                .WillOnce(testing::WithArg<0>(testing::Invoke([this](std::size_t configVariableIndex) {
                    EXPECT_FALSE(configVariableIndexes.contains(configVariableIndex));
                    configVariableIndexes.insert(configVariableIndex);
                })));
            valueSetter(bool{}); 
        })));

    EXPECT_CALL(mockConfigConversion, uint(testing::_, testing::_, testing::_))
        .WillRepeatedly(testing::WithArg<1>(testing::Invoke([this](auto valueSetter) {
            EXPECT_CALL(mockConfig, setVariableWithoutAutoSave(testing::_, testing::_))
                .WillOnce(testing::WithArg<0>(testing::Invoke([this](std::size_t configVariableIndex) {
                    EXPECT_FALSE(configVariableIndexes.contains(configVariableIndex));
                    configVariableIndexes.insert(configVariableIndex);
                })));
            valueSetter(std::uint64_t{}); 
        })));

    configSchema.performConversion(mockConfigConversion);
    EXPECT_EQ(configVariableIndexes.size(), ConfigVariableTypes::size());
}

TEST_F(ConfigSchemaTest, EachConfigVariableIsSavedOnce) {
    EXPECT_CALL(mockConfigConversion, beginRoot());
    EXPECT_CALL(mockConfigConversion, endRoot());
    EXPECT_CALL(mockConfigConversion, beginObject(testing::_)).Times(testing::AnyNumber());
    EXPECT_CALL(mockConfigConversion, endObject()).Times(testing::AnyNumber());

    EXPECT_CALL(mockHookContext, config()).WillRepeatedly(testing::ReturnRef(mockConfig));

    EXPECT_CALL(mockConfigConversion, boolean(testing::_, testing::_, testing::_))
        .WillRepeatedly(testing::WithArg<2>(testing::Invoke([this](auto valueGetter) {
            EXPECT_CALL(mockConfig, getVariable(testing::_))
                .WillOnce(testing::WithArg<0>(testing::Invoke([this](std::size_t configVariableIndex) {
                    EXPECT_FALSE(configVariableIndexes.contains(configVariableIndex));
                    configVariableIndexes.insert(configVariableIndex);
                    return std::any{};
            })));
            valueGetter();
        })));

    EXPECT_CALL(mockConfigConversion, uint(testing::_, testing::_, testing::_))
        .WillRepeatedly(testing::WithArg<2>(testing::Invoke([this](auto valueGetter) {
            EXPECT_CALL(mockConfig, getVariable(testing::_))
                .WillOnce(testing::WithArg<0>(testing::Invoke([this](std::size_t configVariableIndex) {
                    EXPECT_FALSE(configVariableIndexes.contains(configVariableIndex));
                    configVariableIndexes.insert(configVariableIndex);
                    return std::any{};
            })));
            valueGetter(); 
        })));

    configSchema.performConversion(mockConfigConversion);
    EXPECT_EQ(configVariableIndexes.size(), ConfigVariableTypes::size());
}
