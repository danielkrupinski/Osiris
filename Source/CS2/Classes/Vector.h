#pragma once

namespace cs2
{

struct Vector {
    float x;
    float y;
    float z;

    [[nodiscard]] bool operator==(const Vector&) const = default;
};

}
