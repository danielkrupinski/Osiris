#pragma once

#include "../SDK/UserCmd.h"

namespace Misc {
    void bunnyHop(UserCmd*) noexcept;
    void disablePostProcessing() noexcept;
    void skybox() noexcept;
    void clanTag() noexcept;
    void colorWorld() noexcept;
    void inverseRagdollGravity() noexcept;
    void reduceFlashEffect() noexcept;
    void removeBlood() noexcept;
    void removeCrouchCooldown(UserCmd*) noexcept;
    void removeSmoke() noexcept;
}
