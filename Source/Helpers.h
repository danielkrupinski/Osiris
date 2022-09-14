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

#include "SDK/WeaponId.h"

struct Color3;
struct Color4;
struct Vector;

namespace Helpers
{
    unsigned int calculateColor(Color4 color) noexcept;
    unsigned int calculateColor(Color3 color) noexcept;
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

    [[nodiscard]] std::size_t calculateVmtLength(const std::uintptr_t* vmt) noexcept;

    constexpr auto isKnife(WeaponId id) noexcept
    {
        return (id >= WeaponId::Bayonet && id <= WeaponId::SkeletonKnife) || id == WeaponId::KnifeT || id == WeaponId::Knife;
    }

    constexpr auto isSouvenirToken(WeaponId id) noexcept
    {
        switch (id) {
        case WeaponId::Berlin2019SouvenirToken:
        case WeaponId::Stockholm2021SouvenirToken:
        case WeaponId::Antwerp2022SouvenirToken:
            return true;
        default:
            return false;
        }
    }

    constexpr auto bronzeEventCoinFromViewerPass(WeaponId id) noexcept
    {
        switch (id) {
        case WeaponId::Katowice2019ViewerPass: return WeaponId::Katowice2019BronzeCoin;
        case WeaponId::Berlin2019ViewerPass:
        case WeaponId::Berlin2019ViewerPassWith3Tokens: return WeaponId::Berlin2019BronzeCoin;
        case WeaponId::Stockholm2021ViewerPass:
        case WeaponId::Stockholm2021ViewerPassWith3Tokens: return WeaponId::Stockholm2021BronzeCoin;
        case WeaponId::Antwerp2022ViewerPass:
        case WeaponId::Antwerp2022ViewerPassWith3Tokens: return WeaponId::Antwerp2022BronzeCoin;
        default: return WeaponId::None;
        }
    }

    constexpr std::uint8_t numberOfTokensWithViewerPass(WeaponId id) noexcept
    {
        switch (id) {
        case WeaponId::Berlin2019ViewerPassWith3Tokens: 
        case WeaponId::Stockholm2021ViewerPassWith3Tokens:
        case WeaponId::Antwerp2022ViewerPassWith3Tokens: return 3;
        default: return 0;
        }
    }

    bool worldToScreen(const Vector& worldPosition, ImVec2& screenPosition) noexcept;
    bool worldToScreenPixelAligned(const Vector& worldPosition, ImVec2& screenPosition) noexcept;

    [[nodiscard]] constexpr auto isMP5LabRats(WeaponId weaponID, int paintKit) noexcept
    {
        return weaponID == WeaponId::Mp5sd && paintKit == 800;
    }

    class RandomGenerator {
    public:
        using GeneratorType = std::mt19937;
        using result_type = GeneratorType::result_type;

        static constexpr auto min()
        {
            return GeneratorType::min();
        }

        static constexpr auto max()
        {
            return GeneratorType::max();
        }

        auto operator()() const
        {
            std::scoped_lock lock{ mutex };
            return gen();
        }

        template <typename Distribution>
        auto operator()(Distribution&& distribution) const
        {
            std::scoped_lock lock{ mutex };
            return distribution(gen);
        }

        template <std::integral T>
        [[nodiscard]] static T random(T min, T max) noexcept
        {
            std::scoped_lock lock{ mutex };
            return std::uniform_int_distribution{ min, max }(gen);
        }

        template <std::floating_point T>
        [[nodiscard]] static T random(T min, T max) noexcept
        {
            std::scoped_lock lock{ mutex };
            return std::uniform_real_distribution{ min, max }(gen);
        }

        template <typename T>
        [[nodiscard]] static std::enable_if_t<std::is_enum_v<T>, T> random(T min, T max) noexcept
        {
            return static_cast<T>(random(static_cast<std::underlying_type_t<T>>(min), static_cast<std::underlying_type_t<T>>(max)));
        }

    private:
        inline static GeneratorType gen{ std::random_device{}() };
        inline static std::mutex mutex;
    };

    template <typename T>
    [[nodiscard]] T random(T min, T max) noexcept
    {
        return RandomGenerator::random(min, max);
    }

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
