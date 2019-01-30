#pragma once

#include "../SDK/UserCmd.h"

namespace Misc {
    void bunnyHop(UserCmd*) noexcept;
    void disablePostProcessing() noexcept;
    void skybox() noexcept;
    void setClanTag(const std::string&) noexcept;
    void colorWorld() noexcept;
    void inverseRagdollGravity() noexcept;
    void removeCrouchCooldown(UserCmd*) noexcept;
    void autoPistol(UserCmd*) noexcept;
    void revealRanks() noexcept;
    void animateClanTag(const std::string_view = { }) noexcept;
}
