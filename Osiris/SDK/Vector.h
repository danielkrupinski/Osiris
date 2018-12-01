#pragma once

struct Vector {
    Vector(const float* array)
    {
        x = array[0];
        y = array[1];
        z = array[2];
    }
    float x, y, z;
};
