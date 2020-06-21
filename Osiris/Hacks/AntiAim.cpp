#include "AntiAim.h"
#include "../Interfaces.h"
#include "../SDK/Engine.h"
#include "../SDK/Entity.h"
#include "../SDK/EntityList.h"
#include "../SDK/NetworkChannel.h"
#include "../SDK/UserCmd.h"

void AntiAim::run(UserCmd* cmd, const Vector& previousViewAngles, const Vector& currentViewAngles, bool& sendPacket) noexcept
{
    if (config->antiAim.enabled) {
        if (!localPlayer)
            return;

        if (config->antiAim.pitch && cmd->viewangles.x == currentViewAngles.x)
            cmd->viewangles.x = config->antiAim.pitchAngle;

        if (config->antiAim.yaw && cmd->viewangles.y == currentViewAngles.y) {
            cmd->viewangles.y = config->antiAim.yawAngle;
          
        }
    }
}
