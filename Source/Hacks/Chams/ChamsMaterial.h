#pragma once

enum class ChamsMaterial {
    Normal = 0,
    Flat,
    Animated,
    Platinum,
    Glass,
    Crystal,
    Chrome,
    Silver,
    Gold,
    Plastic,
    Glow,
    Pearlescent,
    Metallic
};

[[nodiscard]] constexpr std::string_view toString(ChamsMaterial material) noexcept
{
    switch (material) {
    using enum ChamsMaterial;
    case Normal: return "Normal";
    case Flat: return "Flat";
    case Animated: return "Animated";
    case Platinum: return "Platinum";
    case Glass: return "Glass";
    case Crystal: return "Crystal";
    case Chrome: return "Chrome";
    case Silver: return "Silver";
    case Gold: return "Gold";
    case Plastic: return "Plastic";
    case Glow: return "Glow";
    case Pearlescent: return "Pearlescent";
    case Metallic: return "Metallic";
    default: return "";
    }
}
