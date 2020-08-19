#include <cmath>
#include <tuple>

#include "imgui/imgui.h"

#include "Config.h"
#include "ConfigStructs.h"
#include "GameData.h"
#include "Helpers.h"
#include "Memory.h"
#include "SDK/GlobalVars.h"

static auto rainbowColor(float time, float speed, float alpha) noexcept
{
    return std::array{ std::sin(speed * time) * 0.5f + 0.5f,
                       std::sin(speed * time + 2 * static_cast<float>(M_PI) / 3) * 0.5f + 0.5f,
                       std::sin(speed * time + 4 * static_cast<float>(M_PI) / 3) * 0.5f + 0.5f,
                       alpha };
}

unsigned int Helpers::calculateColor(ColorA color) noexcept
{
   // if (!config->ignoreFlashbang)
        color.color[3] *= (255.0f - GameData::local().flashDuration) / 255.0f;
    return ImGui::ColorConvertFloat4ToU32(color.rainbow ? rainbowColor(memory->globalVars->realtime, color.rainbowSpeed, color.color[3]) : color.color);
}

unsigned int Helpers::calculateColor(int r, int g, int b, int a) noexcept
{
    return IM_COL32(r, g, b, a);
}

ImWchar* Helpers::getFontGlyphRanges() noexcept
{
    static ImVector<ImWchar> ranges;
    if (ranges.empty()) {
        ImFontGlyphRangesBuilder builder;
        constexpr ImWchar baseRanges[]{ 0x0100, 0x024F, 0x0370, 0x03FF, 0x0600, 0x06FF, 0x0E00, 0x0E7F, 0 };
        builder.AddRanges(baseRanges);
        builder.AddRanges(ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());
        builder.AddRanges(ImGui::GetIO().Fonts->GetGlyphRangesChineseSimplifiedCommon());
        builder.AddText("\u9F8D\u738B\u2122");
        builder.BuildRanges(&ranges);
    }
    return ranges.Data;
}
