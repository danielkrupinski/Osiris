#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <Features/Visuals/ViewmodelMod/ViewmodelMod.h>
#include <Mocks/MockBaseEntity.h>
#include <Mocks/MockConfig.h>
#include <Mocks/MockHookContext.h>
#include <Mocks/MockPlayerController.h>

class ViewmodelModTest : public testing::Test {
protected:
    testing::StrictMock<MockHookContext> mockHookContext{};
    testing::StrictMock<MockConfig> mockConfig{};
    testing::StrictMock<MockPlayerController> mockLocalPlayerController{};
    testing::StrictMock<MockBaseEntity> mockBasePawn{};
    testing::StrictMock<MockPlayerPawn> mockLocalPlayerPawn{};
    ViewmodelMod<MockHookContext> viewmodelMod{mockHookContext};
};

struct ViewmodelModIsFovModificationActiveTestParam {
    bool viewmodelModEnabled{};
    bool fovModificationEnabled{};
    bool expectedFovModificationActive{};
};

class ViewmodelModIsFovModificationActiveTest
    : public ViewmodelModTest,
      public testing::WithParamInterface<ViewmodelModIsFovModificationActiveTestParam> {
};

TEST_P(ViewmodelModIsFovModificationActiveTest, FovModificationIsActiveWhenExpected) {
    EXPECT_CALL(mockHookContext, config()).WillRepeatedly(testing::ReturnRef(mockConfig));
    mockConfig.expectGetVariable<viewmodel_mod_vars::Enabled>().WillRepeatedly(testing::Return(GetParam().viewmodelModEnabled));
    mockConfig.expectGetVariable<viewmodel_mod_vars::ModifyFov>().WillRepeatedly(testing::Return(GetParam().fovModificationEnabled));
    EXPECT_EQ(viewmodelMod.fovModificationActive(), GetParam().expectedFovModificationActive);
}

INSTANTIATE_TEST_SUITE_P(, ViewmodelModIsFovModificationActiveTest, testing::ValuesIn(
    std::to_array<ViewmodelModIsFovModificationActiveTestParam>({
        {.viewmodelModEnabled = false, .fovModificationEnabled = false, .expectedFovModificationActive = false},
        {.viewmodelModEnabled = false, .fovModificationEnabled = true, .expectedFovModificationActive = false},
        {.viewmodelModEnabled = true, .fovModificationEnabled = false, .expectedFovModificationActive = false},
        {.viewmodelModEnabled = true, .fovModificationEnabled = true, .expectedFovModificationActive = true}
    })
));

struct ViewmodelModShouldModifyFovTestParam {
    bool hasLocalPlayerPawn{};
    Optional<bool> isLocalPlayerScoped{};
    bool shouldModifyViewmodelFov{};
};

class ViewmodelModShouldModifyFovTest
    : public ViewmodelModTest,
      public testing::WithParamInterface<ViewmodelModShouldModifyFovTestParam> {
};

TEST_P(ViewmodelModShouldModifyFovTest, ShouldModifyViewmodelFovWhenExpected) {
    EXPECT_CALL(mockHookContext, localPlayerController()).WillOnce(testing::ReturnRef(mockLocalPlayerController));
    EXPECT_CALL(mockLocalPlayerController, pawn()).WillOnce(testing::ReturnRef(mockBasePawn));
    EXPECT_CALL(mockBasePawn, castToPlayerPawn()).WillOnce(testing::ReturnRef(mockLocalPlayerPawn));
    EXPECT_CALL(mockLocalPlayerPawn, operatorBool()).WillRepeatedly(testing::Return(GetParam().hasLocalPlayerPawn));
    EXPECT_CALL(mockLocalPlayerPawn, isScoped()).WillRepeatedly(testing::Return(GetParam().isLocalPlayerScoped));
    EXPECT_EQ(viewmodelMod.shouldModifyViewmodelFov(), GetParam().shouldModifyViewmodelFov);
}

INSTANTIATE_TEST_SUITE_P(, ViewmodelModShouldModifyFovTest, testing::ValuesIn(
    std::to_array<ViewmodelModShouldModifyFovTestParam>({
        {.hasLocalPlayerPawn = false, .shouldModifyViewmodelFov = false},
        {.hasLocalPlayerPawn = true, .isLocalPlayerScoped{true}, .shouldModifyViewmodelFov = false},
        {.hasLocalPlayerPawn = true, .isLocalPlayerScoped{false}, .shouldModifyViewmodelFov = true},
        {.hasLocalPlayerPawn = true, .isLocalPlayerScoped{std::nullopt}, .shouldModifyViewmodelFov = true}
    })
));

struct ViewmodelModFovTestParam {
    viewmodel_mod_vars::Fov::ValueType::ValueType configuredFov{};
    float expectedFov{};
};

class ViewmodelModFovTest
    : public ViewmodelModTest,
      public testing::WithParamInterface<ViewmodelModFovTestParam> {
};

TEST_P(ViewmodelModFovTest, CorrectViewmodelFovIsReturned) {
    EXPECT_CALL(mockHookContext, config()).WillOnce(testing::ReturnRef(mockConfig));
    mockConfig.expectGetVariable<viewmodel_mod_vars::Fov>(viewmodel_mod_vars::Fov::ValueType{GetParam().configuredFov});
    EXPECT_EQ(viewmodelMod.viewmodelFov(), GetParam().expectedFov);
}

static_assert(viewmodel_mod_vars::Fov::ValueType::kMin == 40, "Update the test below");
static_assert(viewmodel_mod_vars::Fov::ValueType::kMax == 90, "Update the test below");
static_assert(viewmodel_mod_vars::Fov::kDefaultValue == 80, "Update the test below");

INSTANTIATE_TEST_SUITE_P(, ViewmodelModFovTest, testing::ValuesIn(
    std::to_array<ViewmodelModFovTestParam>({
        {.configuredFov = 40, .expectedFov = 40.0f}, 
        {.configuredFov = 90, .expectedFov = 90.0f}, 
        {.configuredFov = 80, .expectedFov = 80.0f}
    })
));
