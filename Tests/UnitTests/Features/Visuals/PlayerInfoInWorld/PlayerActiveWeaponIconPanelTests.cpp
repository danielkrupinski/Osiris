#include <string_view>

#include <gtest/gtest.h>

#include <Features/Visuals/PlayerInfoInWorld/PlayerInfoInWorldConfigVariables.h>
#include <Features/Visuals/PlayerInfoInWorld/PlayerWeaponIcon/ActiveWeaponIcon/PlayerActiveWeaponIconPanel.h>

namespace {

struct FakeImagePanel {
    [[nodiscard]] std::string_view getImagePath() const noexcept { return imagePath; }

    void setImageSvg(const char* path, int height) noexcept
    {
        auto* destination = imagePath;
        while (*path)
            *destination++ = *path++;
        *destination = '\0';
        textureHeight = height;
        ++setImageCalls;
    }

    char imagePath[100]{};
    int textureHeight{};
    int setImageCalls{};
};

struct FakeClientPanel {
    template <template <typename...> typename>
    [[nodiscard]] FakeImagePanel& as() const noexcept { return imagePanel; }

    FakeImagePanel& imagePanel;
};

struct FakePanel {
    void setVisible(bool newVisibility) noexcept { visible = newVisibility; }
    [[nodiscard]] FakeClientPanel clientPanel() const noexcept { return {imagePanel}; }

    FakeImagePanel& imagePanel;
    bool visible{true};
};

struct FakeConfig {
    template <typename>
    [[nodiscard]] bool getVariable() const noexcept { return activeWeaponIconEnabled; }

    bool activeWeaponIconEnabled{true};
};

struct FakeWeapon {
    [[nodiscard]] bool isC4() const noexcept { return c4; }
    [[nodiscard]] const char* getName() const noexcept { return name; }

    const char* name{};
    bool c4{};
};

struct FakePlayerPawn {
    [[nodiscard]] FakeWeapon getActiveWeapon() const noexcept { return activeWeapon; }

    FakeWeapon activeWeapon{};
};

struct ActiveWeaponIconContext {
    [[nodiscard]] FakeConfig& config() const noexcept { return config_; }
    [[nodiscard]] FakePanel& panel() const noexcept { return panel_; }

    FakeConfig& config_;
    FakePanel& panel_;
};

class PlayerActiveWeaponIconPanelTest : public testing::Test {
protected:
    FakeImagePanel imagePanel;
    FakePanel panel{imagePanel};
    FakeConfig config;
    ActiveWeaponIconContext context{config, panel};
    PlayerActiveWeaponIconPanel<void, ActiveWeaponIconContext> activeWeaponIconPanel{context};
    FakePlayerPawn playerPawn;
};

TEST_F(PlayerActiveWeaponIconPanelTest, DisabledIconHidesPanel)
{
    config.activeWeaponIconEnabled = false;
    activeWeaponIconPanel.update(playerPawn, Visibility::Hidden);
    EXPECT_FALSE(panel.visible);
}

TEST_F(PlayerActiveWeaponIconPanelTest, NullWeaponNameHidesPanel)
{
    activeWeaponIconPanel.update(playerPawn, Visibility::Hidden);
    EXPECT_FALSE(panel.visible);
}

TEST_F(PlayerActiveWeaponIconPanelTest, InvalidWeaponPrefixHidesPanel)
{
    playerPawn.activeWeapon.name = "ak47";
    activeWeaponIconPanel.update(playerPawn, Visibility::Hidden);
    EXPECT_FALSE(panel.visible);
}

TEST_F(PlayerActiveWeaponIconPanelTest, ValidWeaponNameShowsPanelAndSetsEquipmentIcon)
{
    playerPawn.activeWeapon.name = "weapon_ak47";
    activeWeaponIconPanel.update(playerPawn, Visibility::Hidden);
    EXPECT_TRUE(panel.visible);
    EXPECT_STREQ(imagePanel.imagePath, "s2r://panorama/images/icons/equipment/ak47.svg");
    EXPECT_EQ(imagePanel.textureHeight, 24);
}

TEST_F(PlayerActiveWeaponIconPanelTest, VisibleBombIconHidesC4ActiveWeaponIcon)
{
    playerPawn.activeWeapon = {.name = "weapon_c4", .c4 = true};
    activeWeaponIconPanel.update(playerPawn, Visibility::Visible);
    EXPECT_FALSE(panel.visible);
    EXPECT_EQ(imagePanel.setImageCalls, 0);
}

}
