#pragma once

#include "Vector.h"

namespace csgo
{

struct matrix3x4 {
    constexpr auto operator[](int i) const noexcept { return mat[i]; }
    constexpr auto origin() const noexcept;
private:
    float mat[3][4];
};

constexpr auto matrix3x4::origin() const noexcept
{
    return Vector{ mat[0][3], mat[1][3], mat[2][3] };
}

}

constexpr auto transform(const csgo::Vector& v, const csgo::matrix3x4& mat) noexcept
{
    return csgo::Vector{ v.dotProduct({ mat[0][0], mat[0][1], mat[0][2] }) + mat[0][3],
                    v.dotProduct({ mat[1][0], mat[1][1], mat[1][2] }) + mat[1][3],
                    v.dotProduct({ mat[2][0], mat[2][1], mat[2][2] }) + mat[2][3] };
}