#include "AntiAim.h"
#include "../Interfaces.h"
#include "../SDK/Engine.h"
#include "../SDK/Entity.h"
#include "../SDK/EntityList.h"
#include "../SDK/NetworkChannel.h"
#include "../SDK/UserCmd.h"
#include "../SDK/GlobalVars.h"

void AntiAim::run(UserCmd* cmd, bool& sendPacket) noexcept
{
    if (config.antiAim.enabled) {
        bool lbyFlick{ false };
        static auto lastYaw{ 0.0f };
        static auto pitchDance{ 0.0f };
        static bool yawFlick{ false };
        static bool lbyBreak{ false };
        static float lastCheck{ 0.0f };

        if (config.antiAim.lbyBreaker) {
            if (auto localPlayer = interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer()); localPlayer->vecVelocity().length2D() >= 0.1f || !(localPlayer->flags() & 1) || localPlayer->flags() & 32) {
                lbyBreak = false;
                lastCheck = memory.globalVars->currenttime;
            }
            else {
                if (!lbyBreak && (memory.globalVars->serverTime() - lastCheck) > 0.22f) {
                    cmd->viewangles.y -= config.antiAim.lbyBreakerOffset;
                    lbyBreak = true;
                    lastCheck = memory.globalVars->currenttime;
                    lbyFlick = true;
                }
                else if (lbyBreak && (memory.globalVars->serverTime() - lastCheck) > 1.1f) {
                    cmd->viewangles.y -= config.antiAim.lbyBreakerOffset;
                    lbyBreak = true;
                    lastCheck = memory.globalVars->currenttime;
                    lbyFlick = true;
                }
            }
        }

        if (!lbyFlick) {

            const auto maxDesyncAngle{ interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer())->getMaxDesyncAngle() };

            if (!sendPacket) {
                cmd->viewangles.normalize();
                lastYaw = std::clamp(cmd->viewangles.y, -180.0f, 180.0f);

                switch (config.antiAim.realYawType) {
                case 1:
                    cmd->viewangles.y = lastYaw - maxDesyncAngle;
                    break;
                case 2:
                    cmd->viewangles.y = lastYaw + maxDesyncAngle;
                    break;
                case 3:
                    yawFlick = !yawFlick;
                    cmd->viewangles.y = lastYaw - yawFlick ? maxDesyncAngle : -maxDesyncAngle;
                    break;
                default:
                    break;
                }

            }
            else {
                switch (config.antiAim.fakeYawType) {
                case 1:
                    cmd->viewangles.y = lastYaw - maxDesyncAngle;
                    break;
                case 2:
                    cmd->viewangles.y = lastYaw + maxDesyncAngle;
                    break;
                case 3:
                    yawFlick = !yawFlick;
                    cmd->viewangles.y = lastYaw - yawFlick ? maxDesyncAngle : -maxDesyncAngle;
                    break;
                default:
                    break;
                }
            }
        }

        switch (config.antiAim.realPitchType) {
        case 1:
            cmd->viewangles.x = -89.0f;
            break;
        case 2:
            cmd->viewangles.x = 89.0f;
            break;
        case 3:
            pitchDance += 45.0f;
            if (pitchDance > 100.f)
                pitchDance = 0.0f;
            cmd->viewangles.x = (pitchDance < 75.f) ? 89.f : -89.f;
            break;
        case 4:
            cmd->viewangles.x = 0.0f;
        default:
            break;
        }
    }
}
