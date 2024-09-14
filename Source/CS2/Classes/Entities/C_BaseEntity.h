#pragma once

#include <cstdint>

#include <CS2/Classes/EntitySystem/CEntityHandle.h>
#include "CEntityInstance.h"

namespace cs2
{

struct CEntitySubclassVDataBase;
struct CGameSceneNode;
struct CRenderComponent;

struct C_BaseEntity : CEntityInstance {
    using m_pGameSceneNode = CGameSceneNode*;
    using m_iHealth = std::int32_t;
    using m_lifeState = std::uint8_t;
    using m_iTeamNum = std::uint8_t;
    using m_pSubclassVData = CEntitySubclassVDataBase*;
    using m_pRenderComponent = CRenderComponent*;
    using m_hOwnerEntity = CEntityHandle;
};

}
