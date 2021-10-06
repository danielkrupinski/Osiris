#pragma once

#include "Entity.h"
#include "Inconstructible.h"

class PlantedC4 : public Entity {
public:
    INCONSTRUCTIBLE(PlantedC4)

    NETVAR(c4BlowTime, "CPlantedC4", "m_flC4Blow", float)
    NETVAR(c4TimerLength, "CPlantedC4", "m_flTimerLength", float)
    NETVAR(c4BombSite, "CPlantedC4", "m_nBombSite", int)
    NETVAR(c4Ticking, "CPlantedC4", "m_bBombTicking", bool)
    NETVAR(c4DefuseCountDown, "CPlantedC4", "m_flDefuseCountDown", float)
    NETVAR(c4DefuseLength, "CPlantedC4", "m_flDefuseLength", float)
    NETVAR(c4Defuser, "CPlantedC4", "m_hBombDefuser", int)
};
