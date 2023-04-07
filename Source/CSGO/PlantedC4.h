#pragma once

#include "Entity.h"
#include "VirtualMethod.h"

namespace csgo
{

struct PlantedC4POD;

struct PlantedC4 : GameClass<PlantedC4, PlantedC4POD> {
    [[nodiscard]] Entity getEntity() const
    {
        return Entity::from(getInvoker(), reinterpret_cast<EntityPOD*>(getPOD()));
    }

    NETVAR2(c4BlowTime, "CPlantedC4", "m_flC4Blow", float)
    NETVAR2(c4TimerLength, "CPlantedC4", "m_flTimerLength", float)
    NETVAR2(c4BombSite, "CPlantedC4", "m_nBombSite", int)
    NETVAR2(c4Ticking, "CPlantedC4", "m_bBombTicking", bool)
    NETVAR2(c4DefuseCountDown, "CPlantedC4", "m_flDefuseCountDown", float)
    NETVAR2(c4DefuseLength, "CPlantedC4", "m_flDefuseLength", float)
    NETVAR2(c4Defuser, "CPlantedC4", "m_hBombDefuser", int)
};

}
