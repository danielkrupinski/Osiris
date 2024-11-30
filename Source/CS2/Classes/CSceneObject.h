#pragma once

#include <cstdint>

namespace cs2
{

enum SceneObjectFlags {
    SceneObjectFlag_IsDeleted = 0x20
};

enum ESceneObjectFlags : std::uint64_t {
    SCENEOBJECTFLAG_PIPELINE_SPECIFIC_2 = (std::uint64_t{1} << 54)
};

struct SceneObjectAttributes {
    struct FloatAttributes {};

    FloatAttributes floatAttributes;
};

struct CSceneObject {
    using m_nObjectClass = std::uint8_t;
    using flags = std::uint8_t;
    using attributes = SceneObjectAttributes*;
    using m_nRenderableFlags = std::uint64_t;
};

}
