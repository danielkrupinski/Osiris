#pragma once

#include <array>

#include "imgui/imgui.h"
#include "Config.h"

struct ColorA;

namespace Helpers
{
    unsigned int calculateColor(ColorA color) noexcept;
    unsigned int calculateColor(Config::Color color) noexcept;
    unsigned int calculateColor(int r, int g, int b, int a) noexcept;

    constexpr auto units2meters(float units) noexcept
    {
        return units * 0.0254f;
    }

    ImWchar* getFontGlyphRanges() noexcept;

    constexpr int utf8SeqLen(char firstByte) noexcept
    {
        return (firstByte & 0x80) == 0x00 ? 1 :
               (firstByte & 0xE0) == 0xC0 ? 2 :
               (firstByte & 0xF0) == 0xE0 ? 3 :
               (firstByte & 0xF8) == 0xF0 ? 4 :
               -1;
    }

    constexpr auto utf8Substr(char* start, char* end, int n) noexcept
    {
        while (start < end && --n)
            start += utf8SeqLen(*start);
        return start;
    }

    inline constexpr std::array skyboxList{ "Default", "cs_baggage_skybox_", "cs_tibet", "embassy", "italy", "jungle", "nukeblank", "office", "sky_cs15_daylight01_hdr", "sky_cs15_daylight02_hdr", "sky_cs15_daylight03_hdr", "sky_cs15_daylight04_hdr", "sky_csgo_cloudy01", "sky_csgo_night_flat", "sky_csgo_night02", "sky_day02_05_hdr", "sky_day02_05", "sky_dust", "sky_l4d_rural02_ldr", "sky_venice", "vertigo_hdr", "vertigo", "vertigoblue_hdr", "vietnam", "sky_lunacy", "sky_hr_aztec" };
}
