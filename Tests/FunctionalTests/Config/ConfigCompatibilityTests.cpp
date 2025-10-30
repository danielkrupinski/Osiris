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

        get<model_glow_vars::Enabled>() = true;
        get<model_glow_vars::GlowPlayers>() = true;
        get<model_glow_vars::GlowOnlyEnemies>() = true;
        get<model_glow_vars::PlayerGlowColorMode>() = PlayerModelGlowColorType::PlayerOrTeamColor;
        get<model_glow_vars::GlowWeapons>() = true;
        get<model_glow_vars::GlowDroppedBomb>() = false;
        get<model_glow_vars::GlowTickingBomb>() = false;
        get<model_glow_vars::GlowDefuseKits>() = true;
        get<model_glow_vars::GlowGrenadeProjectiles>() = true;

        get<outline_glow_vars::Enabled>() = true;
        get<outline_glow_vars::GlowPlayers>() = true;
        get<outline_glow_vars::GlowOnlyEnemies>() = false;
        get<outline_glow_vars::PlayerGlowColorMode>() = PlayerOutlineGlowColorType::HealthBased;
        get<outline_glow_vars::GlowWeapons>() = true;
        get<outline_glow_vars::GlowDroppedBomb>() = true;
        get<outline_glow_vars::GlowTickingBomb>() = false;
        get<outline_glow_vars::GlowDefuseKits>() = true;
        get<outline_glow_vars::GlowHostages>() = true;
        get<outline_glow_vars::GlowGrenadeProjectiles>() = true;

        get<player_info_vars::Enabled>() = true;
        get<player_info_vars::OnlyEnemies>() = true;
        get<player_info_vars::PlayerPositionArrowEnabled>() = true;
        get<player_info_vars::PlayerPositionArrowColorMode>() = PlayerPositionArrowColorType::PlayerOrTeamColor;
        get<player_info_vars::PlayerHealthEnabled>() = true;
        get<player_info_vars::PlayerHealthColorMode>() = PlayerHealthTextColor::HealthBased;
        get<player_info_vars::ActiveWeaponIconEnabled>() = true;
        get<player_info_vars::BombCarrierIconEnabled>() = true;
        get<player_info_vars::BombPlantIconEnabled>() = true;
        get<player_info_vars::ActiveWeaponAmmoEnabled>() = false;
        get<player_info_vars::BombDefuseIconEnabled>() = true;
        get<player_info_vars::HostagePickupIconEnabled>() = true;
        get<player_info_vars::HostageRescueIconEnabled>() = true;
        get<player_info_vars::BlindedIconEnabled>() = true;

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
        get<model_glow_vars::PlayerBlueHue>() = model_glow_vars::PlayerBlueHue::ValueType{color::HueInteger{203}};
        get<model_glow_vars::PlayerGreenHue>() = model_glow_vars::PlayerGreenHue::ValueType{color::HueInteger{133}};
        get<model_glow_vars::PlayerYellowHue>() = model_glow_vars::PlayerYellowHue::ValueType{color::HueInteger{48}};
        get<model_glow_vars::PlayerOrangeHue>() = model_glow_vars::PlayerOrangeHue::ValueType{color::HueInteger{13}};
        get<model_glow_vars::PlayerPurpleHue>() = model_glow_vars::PlayerPurpleHue::ValueType{color::HueInteger{269}};
    }

    void setVariableExpectationsV3()
    {
        setVariableExpectationsV2();
        get<model_glow_vars::TeamTHue>() = model_glow_vars::TeamTHue::ValueType{color::HueInteger{30}};
        get<model_glow_vars::TeamCTHue>() = model_glow_vars::TeamCTHue::ValueType{color::HueInteger{220}};
        get<model_glow_vars::LowHealthHue>() = model_glow_vars::LowHealthHue::ValueType{color::HueInteger{311}};
        get<model_glow_vars::HighHealthHue>() = model_glow_vars::HighHealthHue::ValueType{color::HueInteger{256}};
        get<model_glow_vars::EnemyHue>() = model_glow_vars::EnemyHue::ValueType{color::HueInteger{353}};
        get<model_glow_vars::AllyHue>() = model_glow_vars::EnemyHue::ValueType{color::HueInteger{74}};
    }

    void setVariableExpectationsV4()
    {
        setVariableExpectationsV3();
        get<model_glow_vars::MolotovHue>() = model_glow_vars::MolotovHue::ValueType{color::HueInteger{37}};
        get<model_glow_vars::FlashbangHue>() = model_glow_vars::FlashbangHue::ValueType{color::HueInteger{205}};
        get<model_glow_vars::HEGrenadeHue>() = model_glow_vars::HEGrenadeHue::ValueType{color::HueInteger{333}};
        get<model_glow_vars::SmokeGrenadeHue>() = model_glow_vars::SmokeGrenadeHue::ValueType{color::HueInteger{116}};
    }

    void setVariableExpectationsV5()
    {
        setVariableExpectationsV4();
        get<model_glow_vars::DroppedBombHue>() = model_glow_vars::DroppedBombHue::ValueType{color::HueInteger{69}};
        get<model_glow_vars::TickingBombHue>() = model_glow_vars::TickingBombHue::ValueType{color::HueInteger{303}};
        get<model_glow_vars::DefuseKitHue>() = model_glow_vars::DefuseKitHue::ValueType{color::HueInteger{227}};
    }

    void setVariableExpectationsV6()
    {
        setVariableExpectationsV5();
        get<outline_glow_vars::PlayerBlueHue>() = outline_glow_vars::PlayerBlueHue::ValueType{color::HueInteger{200}};
        get<outline_glow_vars::PlayerGreenHue>() = outline_glow_vars::PlayerGreenHue::ValueType{color::HueInteger{134}};
        get<outline_glow_vars::PlayerYellowHue>() = outline_glow_vars::PlayerYellowHue::ValueType{color::HueInteger{57}};
        get<outline_glow_vars::PlayerOrangeHue>() = outline_glow_vars::PlayerOrangeHue::ValueType{color::HueInteger{12}};
        get<outline_glow_vars::PlayerPurpleHue>() = outline_glow_vars::PlayerPurpleHue::ValueType{color::HueInteger{256}};
        get<outline_glow_vars::TeamTHue>() = outline_glow_vars::TeamTHue::ValueType{color::HueInteger{37}};
        get<outline_glow_vars::TeamCTHue>() = outline_glow_vars::TeamCTHue::ValueType{color::HueInteger{227}};
        get<outline_glow_vars::LowHealthHue>() = outline_glow_vars::LowHealthHue::ValueType{color::HueInteger{287}};
        get<outline_glow_vars::HighHealthHue>() = outline_glow_vars::HighHealthHue::ValueType{color::HueInteger{171}};
        get<outline_glow_vars::EnemyHue>() = outline_glow_vars::EnemyHue::ValueType{color::HueInteger{304}};
        get<outline_glow_vars::AllyHue>() = outline_glow_vars::EnemyHue::ValueType{color::HueInteger{103}};
        get<outline_glow_vars::MolotovHue>() = outline_glow_vars::MolotovHue::ValueType{color::HueInteger{60}};
        get<outline_glow_vars::FlashbangHue>() = outline_glow_vars::FlashbangHue::ValueType{color::HueInteger{250}};
        get<outline_glow_vars::HEGrenadeHue>() = outline_glow_vars::HEGrenadeHue::ValueType{color::HueInteger{300}};
        get<outline_glow_vars::SmokeGrenadeHue>() = outline_glow_vars::SmokeGrenadeHue::ValueType{color::HueInteger{140}};
        get<outline_glow_vars::DroppedBombHue>() = outline_glow_vars::DroppedBombHue::ValueType{color::HueInteger{302}};
        get<outline_glow_vars::TickingBombHue>() = outline_glow_vars::TickingBombHue::ValueType{color::HueInteger{26}};
        get<outline_glow_vars::DefuseKitHue>() = outline_glow_vars::DefuseKitHue::ValueType{color::HueInteger{160}};
    }

    void setVariableExpectationsV7()
    {
        setVariableExpectationsV6();
        get<viewmodel_mod_vars::Enabled>() = true;
        get<viewmodel_mod_vars::ModifyFov>() = true;
        get<viewmodel_mod_vars::Fov>() = viewmodel_mod_vars::Fov::ValueType{90};
    }

    void setVariableExpectationsV8()
    {
        setVariableExpectationsV7();
        get<no_scope_inaccuracy_vis_vars::Enabled>() = true;
    }

    void setVariableExpectationsV9()
    {
        setVariableExpectationsV8();
        get<outline_glow_vars::HostageHue>() = outline_glow_vars::HostageHue::ValueType{color::HueInteger{334}};
    }

    void setVariableExpectationsV10()
    {
        setVariableExpectationsV9();
        get<BombPlantAlertEnabled>() = true;
    }

    void setVariableExpectationsCurrent()
    {
        setVariableExpectationsV10();
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

TEST_F(ConfigCompatibilityLoadTest, ConfigV2) {
    setVariableExpectationsV2();
    testLoadingConfigFile("config_v2.cfg");
}

TEST_F(ConfigCompatibilityLoadTest, ConfigV3) {
    setVariableExpectationsV3();
    testLoadingConfigFile("config_v3.cfg");
}

TEST_F(ConfigCompatibilityLoadTest, ConfigV4) {
    setVariableExpectationsV4();
    testLoadingConfigFile("config_v4.cfg");
}

TEST_F(ConfigCompatibilityLoadTest, ConfigV5) {
    setVariableExpectationsV5();
    testLoadingConfigFile("config_v5.cfg");
}

TEST_F(ConfigCompatibilityLoadTest, ConfigV6) {
    setVariableExpectationsV6();
    testLoadingConfigFile("config_v6.cfg");
}

TEST_F(ConfigCompatibilityLoadTest, ConfigV7) {
    setVariableExpectationsV7();
    testLoadingConfigFile("config_v7.cfg");
}

TEST_F(ConfigCompatibilityLoadTest, ConfigV8) {
    setVariableExpectationsV8();
    testLoadingConfigFile("config_v8.cfg");
}

TEST_F(ConfigCompatibilityLoadTest, ConfigV9) {
    setVariableExpectationsV9();
    testLoadingConfigFile("config_v9.cfg");
}

TEST_F(ConfigCompatibilityLoadTest, ConfigCurrent) {
    setVariableExpectationsCurrent();
    testLoadingConfigFile("config_current.cfg");
}
