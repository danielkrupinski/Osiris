#pragma once

#include <CS2/Classes/Entities/C_BaseEntity.h>
#include <CS2/Constants/TeamNumberConstants.h>

enum class TeamNumber : cs2::C_BaseEntity::m_iTeamNum {
    TT = cs2::TEAM_TERRORIST,
    CT = cs2::TEAM_CT
};
