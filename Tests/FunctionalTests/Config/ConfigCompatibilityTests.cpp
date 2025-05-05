#include <filesystem>
#include <fstream>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <Config/Config.h>
#include <Config/ConfigState.h>
#include <Config/ConfigVariableTypes.h>
#include <GlobalContext/OsirisDirectoryPath.h>
#include <Mocks/MockConfig.h>
#include <Mocks/MockGui.h>
#include <Mocks/MockHookContext.h>
#include <Mocks/MockMemoryAllocator.h>

class ConfigCompatibilityTest : public testing::Test {
protected:
    ConfigCompatibilityTest()
    {
        EXPECT_CALL(mockHookContext, configState()).Times(testing::AtLeast(1)).WillRepeatedly(testing::ReturnRef(configState));
        EXPECT_CALL(mockHookContext, config()).Times(testing::AtLeast(1)).WillRepeatedly(testing::ReturnRef(mockConfig));
    }

    [[nodiscard]] static auto readConfigFile(const char* filename)
    {
        std::ifstream file{std::filesystem::path{TEST_CONFIGS_PATH} / filename, std::ios::binary};
        return std::vector<char8_t>{std::istreambuf_iterator<char>{file}, {}};
    }

    template <typename ConfigVariable>
    [[nodiscard]] auto& get()
    {
        return expectedVariableValues[ConfigVariableTypes::indexOf<ConfigVariable>()];
    }

    void setVariableExpectationsV1()
    {
        get<BombTimerEnabled>() = true;
        get<DefusingAlertEnabled>() = true;
        get<KillfeedPreserverEnabled>() = true;
        get<PostRoundTimerEnabled>() = true;

        get<ModelGlowEnabled>() = true;
        get<PlayerModelGlowEnabled>() = true;
        get<PlayerModelGlowOnlyEnemies>() = true;
        get<PlayerModelGlowColorMode>() = PlayerModelGlowColorType::PlayerOrTeamColor;
        get<WeaponModelGlowEnabled>() = true;
        get<DroppedBombModelGlowEnabled>() = false;
        get<TickingBombModelGlowEnabled>() = false;
        get<DefuseKitModelGlowEnabled>() = true;
        get<GrenadeProjectileModelGlowEnabled>() = true;

        get<OutlineGlowEnabled>() = true;
        get<PlayerOutlineGlowEnabled>() = true;
        get<PlayerOutlineGlowOnlyEnemies>() = false;
        get<PlayerOutlineGlowColorMode>() = PlayerOutlineGlowColorType::HealthBased;
        get<WeaponOutlineGlowEnabled>() = true;
        get<DroppedBombOutlineGlowEnabled>() = true;
        get<TickingBombOutlineGlowEnabled>() = false;
        get<DefuseKitOutlineGlowEnabled>() = true;
        get<HostageOutlineGlowEnabled>() = true;
        get<GrenadeProjectileOutlineGlowEnabled>() = true;

        get<PlayerInfoInWorldEnabled>() = true;
        get<PlayerInfoInWorldOnlyEnemies>() = true;
        get<PlayerInfoInWorldPlayerPositionArrowEnabled>() = true;
        get<PlayerInfoInWorldPlayerPositionArrowColorMode>() = PlayerPositionArrowColorType::PlayerOrTeamColor;
        get<PlayerInfoInWorldPlayerHealthEnabled>() = true;
        get<PlayerInfoInWorldPlayerHealthColorMode>() = PlayerHealthTextColor::HealthBased;
        get<PlayerInfoInWorldActiveWeaponIconEnabled>() = true;
        get<PlayerInfoInWorldBombCarrierIconEnabled>() = true;
        get<PlayerInfoInWorldBombPlantIconEnabled>() = true;
        get<PlayerInfoInWorldActiveWeaponAmmoEnabled>() = false;
        get<PlayerInfoInWorldBombDefuseIconEnabled>() = true;
        get<PlayerInfoInWorldHostagePickupIconEnabled>() = true;
        get<PlayerInfoInWorldHostageRescueIconEnabled>() = true;
        get<PlayerInfoInWorldBlindedIconEnabled>() = true;

        get<BombBeepSoundVisualizationEnabled>() = true;
        get<BombDefuseSoundVisualizationEnabled>() = true;
        get<BombPlantSoundVisualizationEnabled>() = true;
        get<FootstepSoundVisualizationEnabled>() = true;
        get<WeaponReloadSoundVisualizationEnabled>() = true;
        get<WeaponScopeSoundVisualizationEnabled>() = true;
    }

    void setVariableExpectationsV2()
    {
        setVariableExpectationsV1();
        get<PlayerModelGlowPlayerBlueHue>() = PlayerModelGlowPlayerBlueHue::ValueType{color::HueInteger{203}};
        get<PlayerModelGlowPlayerGreenHue>() = PlayerModelGlowPlayerGreenHue::ValueType{color::HueInteger{133}};
        get<PlayerModelGlowPlayerYellowHue>() = PlayerModelGlowPlayerYellowHue::ValueType{color::HueInteger{48}};
        get<PlayerModelGlowPlayerOrangeHue>() = PlayerModelGlowPlayerOrangeHue::ValueType{color::HueInteger{13}};
        get<PlayerModelGlowPlayerPurpleHue>() = PlayerModelGlowPlayerPurpleHue::ValueType{color::HueInteger{269}};
    }

    void setVariableExpectationsCurrent()
    {
        setVariableExpectationsV2();
    }

    struct VariableChecker {
        VariableChecker() = default;

        template <typename ValueType>
        explicit(false) VariableChecker(ValueType expectedValue)
            : checker{makeChecker(expectedValue)}
            , expectedValue{expectedValue}
        {
        }

        void operator()(std::size_t variableIndex, std::any actualValue)
        {
            checker(variableIndex, actualValue);
        }

        std::function<void(std::size_t, std::any)> checker{};
        std::any expectedValue{};

    private:
        template <typename ValueType>
        [[nodiscard]] decltype(auto) static makeChecker(ValueType expectedValue)
        {
            return [expectedValue](std::size_t variableIndex, std::any actualValue) {
                EXPECT_EQ(std::any_cast<ValueType>(actualValue), expectedValue) <<
                    "config variable: " << typeName(variableIndex); 
            };
        }

        [[nodiscard]] static const char* typeName(std::size_t variableIndex)
        {
            const char* typeName = "";
            ConfigVariableTypes::forEach([i = std::size_t{0}, variableIndex, &typeName] <typename ConfigVariable> (std::type_identity<ConfigVariable>) mutable {
                if (i++ == variableIndex)
                    typeName = typeid(ConfigVariable).name();
            });
            return typeName;
        }
    };

    void enableVariableExpectationsForGetting()
    {
        for (auto [variableIndex, valueChecker] : expectedVariableValues)
            EXPECT_CALL(mockConfig, getVariable(variableIndex)).WillOnce(testing::Return(valueChecker.expectedValue));
    }

    testing::StrictMock<MockHookContext> mockHookContext;
    testing::StrictMock<MockConfig> mockConfig;
    testing::StrictMock<MockGui> mockGui;

    ConfigState configState;
    Config<MockHookContext> config{mockHookContext};
    std::unordered_map<std::size_t, VariableChecker> expectedVariableValues;
};

TEST_F(ConfigCompatibilityTest, ConfigCurrentFileIsUpToDate) {
    const auto currentConfigFile = readConfigFile("config_current.cfg");

    setVariableExpectationsCurrent();
    enableVariableExpectationsForGetting();

    std::array<char8_t, build::kConfigFileBufferSize> buffer;
    configState.currentFileOperation = ConfigFileOperation::None;
    configState.fileOperationBuffer = buffer.data();
    configState.autoSaveScheduled = true;
    configState.bufferUsedBytes = 0;
    config.update();

    ASSERT_EQ(configState.bufferUsedBytes, currentConfigFile.size())
        << "generated up-to-date config has different length than config_current.cfg file";

    for (std::size_t i = 0; i < configState.bufferUsedBytes; ++i)
        EXPECT_EQ(buffer.at(i), currentConfigFile.at(i)) << "at index: " << i;
}

class ConfigCompatibilityLoadTest : public ConfigCompatibilityTest {
protected:
    ConfigCompatibilityLoadTest()
    {
        EXPECT_CALL(mockHookContext, gui()).WillOnce(testing::ReturnRef(mockGui));
        EXPECT_CALL(mockGui, updateFromConfig());
    }

    void testLoadingConfigFile(const char* filename)
    {
        enableVariableExpectationsForSetting();
        auto fileContents = readConfigFile(filename);
        prepareLoadOperation(fileContents);
        config.update();
    }
    
    void enableVariableExpectationsForSetting()
    {
        for (auto [variableIndex, valueChecker] : expectedVariableValues)
            EXPECT_CALL(mockConfig, setVariableWithoutAutoSave(variableIndex, testing::_)).WillOnce(testing::Invoke(valueChecker));
    }

    void prepareLoadOperation(std::vector<char8_t>& fileContents)
    {
        configState.fileOperationBuffer = fileContents.data();
        configState.currentFileOperation = ConfigFileOperation::Load;
        configState.bufferUsedBytes = fileContents.size();
    }
};

TEST_F(ConfigCompatibilityLoadTest, ConfigV1) {
    setVariableExpectationsV1();
    testLoadingConfigFile("config_v1.cfg");
}

TEST_F(ConfigCompatibilityLoadTest, ConfigCurrent) {
    setVariableExpectationsCurrent();
    testLoadingConfigFile("config_current.cfg");
}
