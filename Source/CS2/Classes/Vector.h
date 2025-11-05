#pragma once

namespace cs2
{

struct Vector {
    float x, y, z;

    [[nodiscard]] constexpr float squareDistTo(const Vector& other) const noexcept
    {
        return (*this - other).squareLength();
    }

    [[nodiscard]] constexpr float squareLength() const noexcept
    {
        return x * x + y * y + z * z;
    }

    [[nodiscard]] friend constexpr Vector operator-(const Vector& a, const Vector& b) noexcept
    {
        return Vector{a.x - b.x, a.y - b.y, a.z - b.z};
    }

    [[nodiscard]] bool operator==(const Vector&) const = default;
};

}
