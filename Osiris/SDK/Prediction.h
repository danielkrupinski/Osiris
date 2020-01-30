#pragma once
#include "../SDK/Utils.h"
#include "../SDK/Vector.h"
#include "../SDK/Entity.h"
#include "../SDK/UserCmd.h"



class MoveHelper
{
public:
    constexpr void SetHost(Entity* host)
    {
        return callVirtualMethod<void, Entity*>(this, 1, host);
    }
};

struct MoveData {
    bool    m_bFirstRunOfFunctions : 1;
    bool    m_bGameCodeMovedPlayer : 1;
    int     m_nPlayerHandle;        // edict index on server, client entity handle on client=
    int     m_nImpulseCommand;      // Impulse command issued.
    Vector  m_vecViewAngles;        // Command view angles (local space)
    Vector  m_vecAbsViewAngles;     // Command view angles (world space)
    int     m_nButtons;             // Attack buttons.
    int     m_nOldButtons;          // From host_client->oldbuttons;
    float   m_flForwardMove;
    float   m_flSideMove;
    float   m_flUpMove;
    float   m_flMaxSpeed;
    float   m_flClientMaxSpeed;
    Vector  m_vecVelocity;          // edict::velocity        // Current movement direction.
    Vector  m_vecAngles;            // edict::angles
    Vector  m_vecOldAngles;
    float   m_outStepHeight;        // how much you climbed this move
    Vector  m_outWishVel;           // This is where you tried 
    Vector  m_outJumpVel;           // This is your jump velocity
    Vector  m_vecConstraintCenter;
    float   m_flConstraintRadius;
    float   m_flConstraintWidth;
    float   m_flConstraintSpeedFactor;
    float   m_flUnknown[5];
    Vector  m_vecAbsOrigin;
};

class GameMovement {
public:

    constexpr void ProcessMovement(Entity* localPlayer, MoveData* move)
    {
        return callVirtualMethod<void, Entity*, MoveData*>(this, 1, localPlayer, move);
    }

    constexpr void StartTrackPredictionErrors(Entity* localPlayer)
    {
        return callVirtualMethod<void, Entity*>(this, 3, localPlayer);
    }

    constexpr void FinishTrackPredictionErrors(Entity* localPlayer)
    {
        return callVirtualMethod<void, Entity*>(this, 4, localPlayer);
    }
};

class Predict
{
    public:
        constexpr void SetupMove(Entity* localPlayer, UserCmd* cmd, MoveHelper* helper, MoveData* move) {
            return callVirtualMethod<void, Entity*, UserCmd*, MoveHelper*, MoveData*>(this, 20, localPlayer, cmd, helper, move);
        }

        constexpr void FinishMove(Entity* localPlayer, UserCmd* cmd, MoveData* move) {
            return callVirtualMethod<void, Entity*, UserCmd*, MoveData*>(this, 21, localPlayer, cmd, move);
        }
};
