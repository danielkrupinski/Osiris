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
        PaintKit(int id, const std::string& name, const std::wstring& nameUpperCase) noexcept : id(id), name(name), nameUpperCase(nameUpperCase) { }
        int id;
        std::string name;
        std::wstring nameUpperCase;

        auto operator<(const PaintKit& other) const noexcept
        {
            return nameUpperCase < other.nameUpperCase;
        }
    };

    inline std::vector<PaintKit> skinKits{ { 0, "-", L"-" } };
    inline std::vector<PaintKit> gloveKits;
    inline std::vector<PaintKit> stickerKits{ { 0, "None", L"NONE" } };
}
