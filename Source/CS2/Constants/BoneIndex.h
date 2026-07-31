#pragma once

#include <cstdint>

namespace cs2
{

enum class BoneIndex : std::int32_t {
    Head = 6,
    Neck = 5,
    Chest = 4,
    Stomach = 2,
    Pelvis = 0,
    // Limb bones for full-body bbox in label export. Community-verified
    // CS2 skeleton indices; may need re-tuning if a game patch changes the
    // skeleton.
    LeftFoot = 24,
    RightFoot = 28,
    LeftHand = 16,
    RightHand = 20,
};

}
