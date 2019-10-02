#pragma once

class matrix3x4 {
    float mat[3][4];
public:
    constexpr auto operator[](int i) noexcept { return mat[i]; }
};
