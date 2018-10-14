#define _USE_MATH_DEFINES
#include <cmath>
#include <Windows.h>

#include "../SDK/Defines.h"
#include "../Config.h"
#include "../Memory.h"
#include "Misc.h"

#define RAD2DEG( x ) ( ( float )( x ) * ( float )( 180.0f / ( float )( M_PI ) ) )
#define DEG2RAD( x  )  ( (float)(x) * (float)(M_PI / 180.f) )
#define CheckIfNonValidNumber(x) (fpclassify(x) == FP_INFINITE || fpclassify(x) == FP_NAN || fpclassify(x) == FP_SUBNORMAL)

float normalizeYaw(float value)
{
    while (value > 180)
        value -= 360.f;

    while (value < -180)
        value += 360.f;
    return value;
}

void vectorAngles(const Vector& vecForward, Vector& vecAngles)
{
    Vector vecView;
    if (vecForward[1] == 0.f && vecForward[0] == 0.f) {
        vecView[0] = 0.f;
        vecView[1] = 0.f;
    }
    else {
        vecView[1] = atan2(vecForward[1], vecForward[0]) * 180.f / M_PI;

        if (vecView[1] < 0.f)
            vecView[1] += 360;

        vecView[2] = sqrt(vecForward[0] * vecForward[0] + vecForward[1] * vecForward[1]);

        vecView[0] = atan2(vecForward[2], vecView[2]) * 180.f / M_PI;
    }

    vecAngles[0] = -vecView[0];
    vecAngles[1] = vecView[1];
    vecAngles[2] = 0.f;
}

void Misc::autoStrafe(UserCmd* cmd)
{
    if (config.misc.autoStrafe) {
        if (GetAsyncKeyState(VK_SPACE)) {
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

            auto velocity = (*memory.localPlayer)->getVelocity();
            velocity.z = 0;

            static auto flip = false;
            auto turn_direction_modifier = (flip) ? 1.f : -1.f;
            flip = !flip;

            if (*((*memory.localPlayer)->getFlags()) & FL_ONGROUND || (*memory.localPlayer)->getMoveType() == MOVETYPE_LADDER)
                return;

            if (cmd->forwardmove > 0.f)
                cmd->forwardmove = 0.f;

            auto velocity_length_2d = velocity.Length2D();

            auto strafe_angle = RAD2DEG(atan(15.f / velocity_length_2d));

            if (strafe_angle > 90.f)
                strafe_angle = 90.f;

            else if (strafe_angle < 0.f)
                strafe_angle = 0.f;

            Vector strafeAngle = cmd->viewangles;
            Vector Buffer(0, strafeAngle.y - old_yaw, 0);
            Buffer.y = normalizeYaw(Buffer.y);

            int yaw_delta = Buffer.y;
            old_yaw = strafeAngle.y;

            if (yaw_delta > 0.f)
                cmd->sidemove = -450.f;

            else if (yaw_delta < 0.f)
                cmd->sidemove = 450.f;

            auto abs_yaw_delta = abs(yaw_delta);

            if (abs_yaw_delta <= strafe_angle || abs_yaw_delta >= 30.f) {
                Vector velocity_angles;
                vectorAngles(velocity, velocity_angles);

                Buffer = Vector(0, strafeAngle.y - velocity_angles.y, 0);
                Buffer.y = normalizeYaw(Buffer.y);
                int velocityangle_yawdelta = Buffer.y;

                auto velocity_degree = get_velocity_degree(velocity_length_2d) * 1.0f;// Clientvariables->Misc.Retrack;

                if (velocityangle_yawdelta <= velocity_degree || velocity_length_2d <= 15.f) {
                    if (-(velocity_degree) <= velocityangle_yawdelta || velocity_length_2d <= 15.f) {
                        strafeAngle.y += (strafe_angle * turn_direction_modifier);
                        cmd->sidemove = 450.f * turn_direction_modifier;
                    }

                    else {
                        strafeAngle.y = velocity_angles.y - velocity_degree;
                        cmd->sidemove = 450.f;
                    }
                }

                else {
                    strafeAngle.y = velocity_angles.y + velocity_degree;
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

void Misc::fixMovement(UserCmd* cmd)
{
    Vector vMove = Vector(cmd->forwardmove, cmd->sidemove, 0.0f);
    float flSpeed = vMove.Length();
    Vector qMove;
    vectorAngles(vMove, qMove);
    float normalized = fmod(cmd->viewangles.y + 180.f, 360.f) - 180.f;
    float normalizedx = fmod(cmd->viewangles.x + 180.f, 360.f) - 180.f;
    Vector strafeAngle = cmd->viewangles;
    float flYaw = DEG2RAD((normalized - strafeAngle.y) + qMove.y);

    if (normalizedx >= 90.0f || normalizedx <= -90.0f || (cmd->viewangles.x >= 90.f && cmd->viewangles.x <= 200) || cmd->viewangles.x <= -90)
        cmd->forwardmove = -cos(flYaw) * flSpeed;
    else
        cmd->forwardmove = cos(flYaw) * flSpeed;

    cmd->sidemove = sin(flYaw) * flSpeed;
}
