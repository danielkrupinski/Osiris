#pragma once

namespace cs2
{

struct VMatrix {
    static consteval VMatrix identity() noexcept
    {
        return VMatrix{{
            {1.0f, 0.0f, 0.0f, 0.0f},
            {0.0f, 1.0f, 0.0f, 0.0f},
            {0.0f, 0.0f, 1.0f, 0.0f},
            {0.0f, 0.0f, 0.0f, 1.0f}
        }};
    }

    float m[4][4];
};

}
