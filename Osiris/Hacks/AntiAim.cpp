#include "AntiAim.h"
#include "../Interfaces.h"
#include "../SDK/Engine.h"
#include "../SDK/Entity.h"
#include "../SDK/EntityList.h"
#include "../SDK/NetworkChannel.h"
#include "../SDK/UserCmd.h"

void AntiAim::run(UserCmd* cmd, const float oldYaw, bool& sendPacket) noexcept
{
    if (config.antiAim.enabled) {
        static auto lastYaw{ 0.0f };

        if (!sendPacket) {
            cmd->viewangles.normalize();
            lastYaw = std::clamp(cmd->viewangles.y, -180.0f, 180.0f);
        } else {
            cmd->viewangles.y = lastYaw + interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer())->getMaxDesyncAngle();
        }
    }
}
