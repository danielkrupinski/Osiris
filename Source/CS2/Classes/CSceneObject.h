#pragma once

#include <cstdint>

namespace cs2
{

enum SceneObjectFlags {
    SceneObjectFlag_IsDeleted = 0x20
};

struct CSceneObject {
    using m_nObjectClass = std::uint8_t;
    using flags = std::uint8_t;
};

}
