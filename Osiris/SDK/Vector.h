#pragma once

class Vector {
public:
    Vector(const float* array)
    {
        x = array[0];
        y = array[1];
        z = array[2];
    }
    float x, y, z;
};
