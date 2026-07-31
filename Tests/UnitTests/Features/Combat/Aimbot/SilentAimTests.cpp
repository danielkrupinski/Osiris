#include <gtest/gtest.h>
#include <Features/Combat/Aimbot/SilentAim.h>

#include <cstddef>
#include <cstdint>

namespace
{

struct MockCreateMoveHook {
    [[nodiscard]] bool isInstalled() const noexcept
    {
        return installed;
    }

    bool installed{};
};

struct MockHooks {
    MockCreateMoveHook createMoveHook;
    float silentAimPitch{};
    float silentAimYaw{};
    bool silentAimActive{};
};

}

TEST(SilentAimTests, AvailabilityFollowsCreateMoveHookInstallation)
{
    MockHooks hooks;

    EXPECT_FALSE(aimbot::silent::isAvailable(hooks));

    hooks.createMoveHook.installed = true;

    EXPECT_TRUE(aimbot::silent::isAvailable(hooks));
}

TEST(SilentAimTests, RequestStoresAnglesForCreateMove)
{
    MockHooks hooks;

    aimbot::silent::request(hooks, -12.5f, 87.25f);

    EXPECT_EQ(hooks.silentAimPitch, -12.5f);
    EXPECT_EQ(hooks.silentAimYaw, 87.25f);
    EXPECT_TRUE(hooks.silentAimActive);
}

TEST(SilentAimTests, CancelRequestClearsPendingSilentAim)
{
    MockHooks hooks;
    hooks.silentAimActive = true;

    aimbot::silent::cancelRequest(hooks);

    EXPECT_FALSE(hooks.silentAimActive);
}

TEST(SilentAimTests, ApplyToUserCmdWritesProtobufViewAngles)
{
    cs2::CUserCmd cmd{};
    cs2::CBaseUserCmdPB baseCmd{};
    cs2::CMsgQAngle viewAngles{};
    cmd.csgoUserCmd().baseCmd = &baseCmd;
    cmd.csgoUserCmd().inputHistory.currentSize = 0;
    baseCmd.viewAngles = &viewAngles;

    const auto readable = [](const void* address, std::size_t size) noexcept {
        if (!address || size == 0)
            return false;
        return true;
    };

    EXPECT_TRUE(aimbot::silent::applyToUserCmd(cmd, -12.0f, 90.0f, readable));

    EXPECT_EQ(viewAngles.pitch, -12.0f);
    EXPECT_EQ(viewAngles.yaw, 90.0f);
    EXPECT_EQ(viewAngles.roll, 0.0f);
    EXPECT_NE((*reinterpret_cast<std::uint32_t*>(
        reinterpret_cast<std::byte*>(&baseCmd) + 0x8) & (1U << 2)), 0U);
    EXPECT_NE((*reinterpret_cast<std::uint32_t*>(
        reinterpret_cast<std::byte*>(&viewAngles) + 0x8) & 0x7), 0U);
    EXPECT_NE((cmd.csgoUserCmd().hasBits & 0x1), 0U);
}

TEST(SilentAimTests, ApplyToUserCmdWritesInputHistoryViewAngles)
{
    cs2::CUserCmd cmd{};
    cs2::CCSGOInputHistoryEntryPB entry{};
    cs2::CMsgQAngle viewAngles{};
    struct Rep {
        int allocatedSize;
        int pad;
        cs2::CCSGOInputHistoryEntryPB* elements[1];
    } rep{.allocatedSize = 1, .pad = 0, .elements = {&entry}};

    entry.viewAngles = &viewAngles;
    cmd.csgoUserCmd().inputHistory.currentSize = 1;
    cmd.csgoUserCmd().inputHistory.rep = &rep;

    const auto readable = [](const void* address, std::size_t size) noexcept {
        if (!address || size == 0)
            return false;
        return true;
    };

    EXPECT_TRUE(aimbot::silent::applyToUserCmd(cmd, 33.0f, -123.0f, readable));

    EXPECT_EQ(viewAngles.pitch, 33.0f);
    EXPECT_EQ(viewAngles.yaw, -123.0f);
    EXPECT_EQ(viewAngles.roll, 0.0f);
    EXPECT_NE((*reinterpret_cast<std::uint32_t*>(
        reinterpret_cast<std::byte*>(&entry) + 0x8) & 0x1), 0U);
}
