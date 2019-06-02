#pragma once

#include "../SDK/FrameStage.h"

class GameEvent;

namespace SkinChanger {
    void run(FrameStage) noexcept;
    void scheduleHudUpdate() noexcept;
    void overrideHudIcon(GameEvent*) noexcept;
}