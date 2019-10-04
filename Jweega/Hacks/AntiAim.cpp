#include "AntiAim.h"
#include "../Interfaces.h"
#include "../SDK/Engine.h"
#include "../SDK/Entity.h"
#include "../SDK/EntityList.h"
#include "../SDK/NetworkChannel.h"
#include "../SDK/UserCmd.h"

void AntiAim::run(UserCmd* cmd, const Vector& previousViewAngles, const Vector& currentViewAngles, bool& sendPacket) noexcept
{
    if (config.antiAim.enabled) {
        if (config.antiAim.pitch && cmd->viewangles.x == currentViewAngles.x)
            cmd->viewangles.x = config.antiAim.pitchAngle;

        if (config.antiAim.yaw && sendPacket && interfaces.engine->getNetworkChannel()->chokedPackets && cmd->viewangles.y == currentViewAngles.y) {
            cmd->viewangles.y = previousViewAngles.y + interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer())->getMaxDesyncAngle();
            if (fabsf(cmd->sidemove) < 5.0f) {
                if (cmd->buttons & UserCmd::IN_DUCK)
                    cmd->sidemove = cmd->tickCount & 1 ? 3.25f : -3.25f;
                else
                    cmd->sidemove = cmd->tickCount & 1 ? 1.1f : -1.1f;
            }
        }
    }
}
