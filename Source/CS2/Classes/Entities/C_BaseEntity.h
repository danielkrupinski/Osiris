#pragma once

#include <cstdint>

#include "CEntityInstance.h"

namespace cs2
{

struct CEntitySubclassVDataBase;
struct CGameSceneNode;

struct C_BaseEntity : CEntityInstance {
    using m_pGameSceneNode = CGameSceneNode*;
    using m_iHealth = std::int32_t;
    using m_iTeamNum = std::uint8_t;
    using m_pSubclassVData = CEntitySubclassVDataBase*;
};

}
