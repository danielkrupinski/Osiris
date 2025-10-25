#include <type_traits>
#include <gtest/gtest.h>
#include <Config/ConfigVariables.h>

class ConfigVariablesTest : public testing::Test {
public:
    template <typename ConfigVariable>
    [[nodiscard]] static constexpr auto getNonDefaultValue()
    {
        if constexpr (std::is_same_v<typename ConfigVariable::ValueType, bool>)
            return !ConfigVariable::kDefaultValue;
        else if constexpr (std::is_enum_v<typename ConfigVariable::ValueType>)
            return typename ConfigVariable::ValueType{static_cast<std::underlying_type_t<typename ConfigVariable::ValueType>>(ConfigVariable::kDefaultValue) + 1};
        else if constexpr (IsRangeConstrained<typename ConfigVariable::ValueType>::value) {
            if (ConfigVariable::kDefaultValue != ConfigVariable::ValueType::kMin)
                return typename ConfigVariable::ValueType{ConfigVariable::ValueType::kMin};
            return typename ConfigVariable::ValueType{ConfigVariable::ValueType::kMax};
        } else
            static_assert(!std::is_same_v<ConfigVariable, ConfigVariable>, "Unsupported type");
    }

    ConfigVariables configVariables;
};

TEST_F(ConfigVariablesTest, EachVariableHasDefaultValueAfterConstruction) {
    ConfigVariableTypes::forEach([this] <typename ConfigVariable> (std::type_identity<ConfigVariable>) {
        EXPECT_EQ(configVariables.getVariableValue<ConfigVariable>(), ConfigVariable::kDefaultValue);
    });
}

TEST_F(ConfigVariablesTest, EachVariableCanBeSetToNonDefaultValue) {
    ConfigVariableTypes::forEach([this] <typename ConfigVariable> (std::type_identity<ConfigVariable>) {
        constexpr auto kNonDefaultValue{getNonDefaultValue<ConfigVariable>()};
        configVariables.storeVariableValue<ConfigVariable>(kNonDefaultValue);
        EXPECT_EQ(configVariables.getVariableValue<ConfigVariable>(), kNonDefaultValue);
    });
}

TEST_F(ConfigVariablesTest, ChangingVariableValueDoesNotAffectOtherVariables) {
    ConfigVariableTypes::forEach([this] <typename ConfigVariable> (std::type_identity<ConfigVariable>) {
        configVariables.storeVariableValue<ConfigVariable>(getNonDefaultValue<ConfigVariable>());
    });

    ConfigVariableTypes::forEach([this] <typename ConfigVariable> (std::type_identity<ConfigVariable>) {
        EXPECT_EQ(configVariables.getVariableValue<ConfigVariable>(), getNonDefaultValue<ConfigVariable>());
    });
}
