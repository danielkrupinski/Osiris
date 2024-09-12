#pragma once

#include "Entities/CEntityInstance.h"
#include "Vector.h"

namespace cs2
{

struct CGameSceneNode {
    using m_pOwner = CEntityInstance*;
    using m_pChild = CGameSceneNode*;
    using m_pNextSibling = CGameSceneNode*;
    using m_vecAbsOrigin = Vector;
};

}
