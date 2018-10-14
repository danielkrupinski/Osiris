#define _USE_MATH_DEFINES
#include <cmath>
#include <Windows.h>

#include "../SDK/Defines.h"
#include "../Config.h"
#include "../Memory.h"
#include "Misc.h"

/* void Misc::autoStrafe(UserCmd* cmd)
{
#define RAD2DEG( x ) ( ( float )( x ) * ( float )( 180.0f / ( float )( M_PI ) ) )
    if (GetAsyncKeyState(VK_SPACE)) {
        auto viewAngles = cmd->
        static int old_yaw;

        auto get_velocity_degree = [](float length_2d) {
            auto tmp = RAD2DEG(atan(30.f / length_2d));

            if (CheckIfNonValidNumber(tmp) || tmp > 90.f)
                return 90.f;

            else if (tmp < 0.f)
                return 0.f;

            else
                return tmp;
        };

        auto velocity = G::LocalPlayer->GetVelocity();
        velocity.z = 0;

        static auto flip = false;
        auto turn_direction_modifier = (flip) ? 1.f : -1.f;
        flip = !flip;

        if (*((*memory.localPlayer)->getFlags()) & FL_ONGROUND || G::LocalPlayer->GetMoveType() == MOVETYPE_LADDER)
            return;

        if (cmd->forwardmove > 0.f)
            cmd->forwardmove = 0.f;

        auto velocity_length_2d = velocity.Length2D();

        auto strafe_angle = RAD2DEG(atan(15.f / velocity_length_2d));

        if (strafe_angle > 90.f)
            strafe_angle = 90.f;

        else if (strafe_angle < 0.f)
            strafe_angle = 0.f;

        Vector Buffer(0, G::StrafeAngle.y - old_yaw, 0);
        Buffer.y = Math::NormalizeYaw(Buffer.y);

        int yaw_delta = Buffer.y;
        old_yaw = G::StrafeAngle.y;

        if (yaw_delta > 0.f)
            G::UserCmd->sidemove = -450.f;

        else if (yaw_delta < 0.f)
            G::UserCmd->sidemove = 450.f;

        auto abs_yaw_delta = abs(yaw_delta);

        if (abs_yaw_delta <= strafe_angle || abs_yaw_delta >= 30.f) {
            Vector velocity_angles;
            Math::VectorAngles(velocity, velocity_angles);

            Buffer = Vector(0, G::StrafeAngle.y - velocity_angles.y, 0);
            Buffer.y = Math::NormalizeYaw(Buffer.y);
            int velocityangle_yawdelta = Buffer.y;

            auto velocity_degree = get_velocity_degree(velocity_length_2d) * Clientvariables->Misc.Retrack;

            if (velocityangle_yawdelta <= velocity_degree || velocity_length_2d <= 15.f) {
                if (-(velocity_degree) <= velocityangle_yawdelta || velocity_length_2d <= 15.f) {
                    G::StrafeAngle.y += (strafe_angle * turn_direction_modifier);
                    G::UserCmd->sidemove = 450.f * turn_direction_modifier;
                }

                else {
                    G::StrafeAngle.y = velocity_angles.y - velocity_degree;
                    G::UserCmd->sidemove = 450.f;
                }
            }

            else {
                G::StrafeAngle.y = velocity_angles.y + velocity_degree;
                cmd->sidemove = -450.f;
            }
        }

        cmd->buttons &= ~(IN_MOVELEFT | IN_MOVERIGHT | IN_FORWARD | IN_BACK);

        if (cmd->sidemove <= 0.0)
            cmd->buttons |= IN_MOVELEFT;
        else
            cmd->buttons |= IN_MOVERIGHT;

        if (cmd->forwardmove <= 0.0)
            cmd->buttons |= IN_BACK;
        else
            cmd->buttons |= IN_FORWARD;

    }
} */

void Misc::autoStrafe(UserCmd* cmd)
{
    if (config.misc.autoStrafe) {
        bool bKeysPressed = true;
        if (GetAsyncKeyState(0x41) || GetAsyncKeyState(0x57) || GetAsyncKeyState(0x53) || GetAsyncKeyState(0x44)) bKeysPressed = false;

        if ((GetAsyncKeyState(VK_SPACE) && !(*((*memory.localPlayer)->getFlags()) & FL_ONGROUND)) && bKeysPressed) {
            cmd->forwardmove = (1550.f * 5) / (*memory.localPlayer)->getVelocity().Length2D();
            cmd->sidemove = (cmd->command_number % 2) == 0 ? -450.f : 450.f;
            if (cmd->forwardmove > 450.f)
                cmd->forwardmove = 450.f;
        }
    }
}


void Misc::bunnyHop(UserCmd* cmd)
{
    if (config.misc.bunnyHop) {
        static auto bJumped = false;
        static auto bFake = false;
        if (!bJumped && bFake) {
            bFake = false;
            cmd->buttons |= 2;
        }
        else if (cmd->buttons & 2) {
            if (*((*memory.localPlayer)->getFlags()) & 1) {
                bJumped = true;
                bFake = true;
            }
            else {
                cmd->buttons &= ~2;
                bJumped = false;
            }
        }
        else {
            bJumped = false;
            bFake = false;
        }
    }
}
