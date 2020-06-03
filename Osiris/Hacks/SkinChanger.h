#pragma once

#include <string>
#include <vector>

enum class FrameStage;
class GameEvent;

namespace SkinChanger
{
    void initializeKits() noexcept;
    void run(FrameStage) noexcept;
    void scheduleHudUpdate() noexcept;
    void overrideHudIcon(GameEvent& event) noexcept;
    void updateStatTrak(GameEvent& event) noexcept;

    struct PaintKit {
        PaintKit(int id, std::string&& name) noexcept : id(id), name(name) { }
        int id;
        std::string name;

        auto operator<(const PaintKit& other) const noexcept
        {
            return name < other.name;
        }
    };

    inline std::vector<PaintKit> skinKits;
    inline std::vector<PaintKit> gloveKits;
    inline std::vector<PaintKit> stickerKits{ {0, "None"} };
}
