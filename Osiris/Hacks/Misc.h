#pragma once

#include "../Memory.h"
#include "../SDK/UserCmd.h"

namespace Misc {
    void bunnyHop(UserCmd*) noexcept;
    void inverseRagdollGravity() noexcept;
    void autoPistol(UserCmd*) noexcept;
    void revealRanks() noexcept;
    void animateClanTag(bool = false, const std::string_view = { }) noexcept;
    void spectatorList() noexcept;

    constexpr void removeCrouchCooldown(UserCmd* cmd) noexcept
    {
        if (config.misc.fastDuck)
            cmd->buttons |= UserCmd::IN_BULLRUSH;
    }

    constexpr void setClanTag(const std::string_view clanTag) noexcept
    {
        reinterpret_cast<void(__fastcall*)(const char*, const char*)>(memory.setClanTag)(clanTag.data(), clanTag.data());
    }

    constexpr void watermark() noexcept
    {
        if (config.misc.watermark) {
            interfaces.surface->setTextColor(51, 153, 255, 255);
            interfaces.surface->setTextPosition(0, 0);
            interfaces.surface->printText(L"Osiris Internal Multihack\nDeveloped by Daniel Krupiński (dk12345)");
        }
    }
}
