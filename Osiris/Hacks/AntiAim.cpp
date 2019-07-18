#include "AntiAim.h"
#include "../Interfaces.h"
#include "../SDK/Engine.h"
#include "../SDK/Entity.h"
#include "../SDK/EntityList.h"
#include "../SDK/NetworkChannel.h"
#include "../SDK/UserCmd.h"

void AntiAim::run(UserCmd* cmd, float oldYaw, bool& sendPacket) noexcept
{
    if (config.antiAim.enabled) {
        if (sendPacket && !interfaces.engine->getNetworkChannel()->chokedPackets)
            sendPacket = false;
 
        if (sendPacket && interfaces.engine->getNetworkChannel()->chokedPackets && !(cmd->buttons & (UserCmd::IN_ATTACK | UserCmd::IN_ATTACK2)))
            cmd->viewangles.y += fabsf(interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer())->getMaxDesyncAngle()) - fabsf(cmd->viewangles.y - oldYaw);
    }
}
