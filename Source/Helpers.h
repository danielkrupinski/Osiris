#pragma once

#include <concepts>
#include <cwctype>
#include <mutex>
#include <numbers>
#include <random>
#include <string>
#include <unordered_map>
#include <vector>

#include "imgui/imgui.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui/imgui_internal.h"

#include "CSGO/WeaponId.h"

struct Color3;
struct Color4;
namespace csgo { struct Vector; }

namespace Helpers
{
    unsigned int calculateColor(float time, Color4 color) noexcept;
    unsigned int calculateColor(float time, Color3 color) noexcept;
    unsigned int calculateColor(int r, int g, int b, int a) noexcept;
    void setAlphaFactor(float newAlphaFactor) noexcept;
    float getAlphaFactor() noexcept;
    void convertHSVtoRGB(float h, float s, float v, float& outR, float& outG, float& outB) noexcept;
    void healthColor(float fraction, float& outR, float& outG, float& outB) noexcept;
    unsigned int healthColor(float fraction) noexcept;

    constexpr auto units2meters(float units) noexcept
    {
        return units * 0.0254f;
    }

    ImWchar* getFontGlyphRanges() noexcept;

    constexpr std::uint8_t utf8SeqLen(char firstByte) noexcept
    {
        return (firstByte & 0x80) == 0x00 ? 1 :
               (firstByte & 0xE0) == 0xC0 ? 2 :
               (firstByte & 0xF0) == 0xE0 ? 3 :
               (firstByte & 0xF8) == 0xF0 ? 4 :
               0;
    }

    std::wstring toWideString(const std::string& str) noexcept;
 
    bool decodeVFONT(std::vector<char>& buffer) noexcept;
    std::vector<char> loadBinaryFile(const std::string& path) noexcept;

    template <typename T> constexpr auto deg2rad(T degrees) noexcept { return degrees * (std::numbers::pi_v<T> / static_cast<T>(180)); }
    template <typename T> constexpr auto rad2deg(T radians) noexcept { return radians * (static_cast<T>(180) / std::numbers::pi_v<T>); }

    constexpr auto isKnife(WeaponId id) noexcept
    {
        return (id >= WeaponId::Bayonet && id <= WeaponId::SkeletonKnife) || id == WeaponId::KnifeT || id == WeaponId::Knife;
    }

    constexpr auto bronzeEventCoinFromViewerPass(WeaponId id) noexcept
    {
        // TODO: auto detect which coin should the viewer pass give
        switch (id) {
        using enum WeaponId;
        case Katowice2019ViewerPass: return Katowice2019BronzeCoin;
        case Berlin2019ViewerPass:
        case Berlin2019ViewerPassWith3Tokens: return Berlin2019BronzeCoin;
        case Stockholm2021ViewerPass:
        case Stockholm2021ViewerPassWith3Tokens: return Stockholm2021BronzeCoin;
        case Antwerp2022ViewerPass:
        case Antwerp2022ViewerPassWith3Tokens: return Antwerp2022BronzeCoin;
        case Rio2022ViewerPass:
        case Rio2022ViewerPassWith3Tokens: return Rio2022BronzeCoin;
        case Paris2023ViewerPass:
        case Paris2023ViewerPassWith3Tokens: return Paris2023BronzeCoin;
        default: return WeaponId{};
        }
    }

    bool worldToScreen(const csgo::Vector& worldPosition, ImVec2& screenPosition) noexcept;
    bool worldToScreenPixelAligned(const csgo::Vector& worldPosition, ImVec2& screenPosition) noexcept;

    [[nodiscard]] constexpr auto isMP5LabRats(WeaponId weaponID, int paintKit) noexcept
    {
        return weaponID == WeaponId::Mp5sd && paintKit == 800;
    }

    class RandomGenerator {
    public:
        using GeneratorType = std::mt19937;
        using result_type = GeneratorType::result_type;

        static constexpr auto (min)()
        {
            return (GeneratorType::min)();
        }

        static constexpr auto (max)()
        {
            return (GeneratorType::max)();
        }

        auto operator()()
        {
            std::scoped_lock lock{ mutex };
            return gen();
        }

        template <typename Distribution>
        auto operator()(Distribution&& distribution)
        {
            std::scoped_lock lock{ mutex };
            return distribution(gen);
        }

    private:
        GeneratorType gen{ std::random_device{}() };
        std::mutex mutex;
    };

    class ToUpperConverter {
    public:
        std::wstring_view toUpper(std::wstring_view string)
        {
            assert(string.length() < buffer.size());
            std::size_t length = 0;
            for (auto c : string)
                buffer[length++] = toUpper(c);
            buffer[length] = '\0';
            return { buffer.data(), length };
        }

    private:
        wchar_t toUpper(wchar_t c)
        {
            if (c >= 'a' && c <= 'z') {
                return c - ('a' - 'A');
            } else if (c > 127) {
                if (const auto it = cache.find(c); it != cache.end()) {
                    return it->second;
                } else {
                    const auto upper = std::towupper(c);
                    cache.emplace(c, upper);
                    return upper;
                }
            }
            return c;
        }

        std::unordered_map<wchar_t, wchar_t> cache;
        std::array<wchar_t, 4096> buffer;
    };
}
