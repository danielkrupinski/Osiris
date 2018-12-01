#pragma once

struct Vector {
    Vector& operator=(const float* array) noexcept
    {
        x = array[0];
        y = array[1];
        z = array[2];
        return *this;
    }
    float x, y, z;
};
