#pragma once

enum class FrameStage;
class GameEvent;

namespace SkinChanger {
    void run(FrameStage) noexcept;
    void scheduleHudUpdate() noexcept;
    void overrideHudIcon(GameEvent*) noexcept;
}