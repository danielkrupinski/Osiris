#include "AntiAim.h"
#include "../Interfaces.h"
#include "../SDK/Engine.h"
#include "../SDK/Entity.h"
#include "../SDK/EntityList.h"
#include "../SDK/NetworkChannel.h"
#include "../SDK/UserCmd.h"
#include "../SDK/GlobalVars.h"
#include "../SDK/Surface.h"

void AntiAim::run(UserCmd* cmd, bool& sendPacket) noexcept
{
    if (config.antiAim.enabled) {
        static auto lastYaw{ 0.0f };
        static auto pitchDance{ 0.0f };
        static auto yawFlick{ 1.0f };

        const auto localPlayer{ interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer()) };
        if (!localPlayer || !localPlayer->isAlive() || localPlayer->moveType() == MoveType::NOCLIP || localPlayer->moveType() == MoveType::LADDER)
            return;

        if (auto activeWeapon{ localPlayer->getActiveWeapon() }; !activeWeapon || (activeWeapon->isGrenade() && activeWeapon->isInThrow()) )
            return;

        if (config.antiAim.lbyBreaker) {
            static auto lbyBreak{ false };
            static auto lastCheck{ 0.0f };

            if (localPlayer->velocity().length2D() >= 0.1f || !(localPlayer->flags() & 1) || localPlayer->flags() & 32) {
                lbyBreak = false;
                lastCheck = memory.globalVars->currenttime;
            }
            else {
                if (!lbyBreak && (memory.globalVars->currenttime - lastCheck) > 0.22f) {
                    sendPacket = false;
                    lbyBreak = true;
                    lastCheck = memory.globalVars->currenttime;
                }
                else if (lbyBreak && (memory.globalVars->currenttime - lastCheck) > 1.1f) {
                    sendPacket = false;
                    lbyBreak = true;
                    lastCheck = memory.globalVars->currenttime;
                }
            }
        }

        if (config.antiAim.yawType == 4 && GetAsyncKeyState(config.antiAim.key) & 1)
            toggle = -toggle;

        const auto maxDesyncAngle{ interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer())->getMaxDesyncAngle() };
        if (!sendPacket) {
            cmd->viewangles.normalize();
            lastYaw = std::clamp(cmd->viewangles.y, -180.0f, 180.0f);

            switch (config.antiAim.yawType) {
            case 1:
                cmd->viewangles.y = lastYaw - maxDesyncAngle;
                break;
            case 2:
                cmd->viewangles.y = lastYaw + maxDesyncAngle;
                break;
            case 3:
                yawFlick = -yawFlick;
                cmd->viewangles.y = lastYaw + (maxDesyncAngle * yawFlick);
                break;
            case 4:
                cmd->viewangles.y = lastYaw - (maxDesyncAngle * toggle);
                break;
            default:
                break;
            }

        }
        else {
            switch (config.antiAim.yawType) {
            case 1:
                cmd->viewangles.y = lastYaw + maxDesyncAngle;
                break;
            case 2:
                cmd->viewangles.y = lastYaw - maxDesyncAngle;
                break;
            case 3:
                yawFlick = -yawFlick;
                cmd->viewangles.y = lastYaw - (maxDesyncAngle * yawFlick);
                break;
            case 4:
                cmd->viewangles.y = lastYaw + (maxDesyncAngle * toggle);
                break;
            default:
                break;
            }
        }

        switch (config.antiAim.pitchType) {
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

void AntiAim::drawYaw() noexcept {
    if (config.antiAim.yawType == 4 && interfaces.engine->isInGame()) {

        if (auto localPlayer{ interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer()) }; !localPlayer->isAlive())
            return;

        constexpr unsigned font{ 0x1e };

        interfaces.surface->setTextFont(font);
        interfaces.surface->setTextColor(sinf(0.6f * memory.globalVars->realtime) * 127 + 128,
            sinf(0.6f * memory.globalVars->realtime + 2.0f) * 127 + 128,
            sinf(0.6f * memory.globalVars->realtime + 4.0f) * 127 + 128,
            255.0f);

        const auto [width, height] { interfaces.surface->getScreenSize() };
        const auto drawPositionY{ height - (height / 12) };
        const auto leftText{ toggle > 0.0f ? L"REAL" : L"FAKE" };
        const auto rightText{ toggle < 0.0f ? L"REAL" : L"FAKE" };

        interfaces.surface->setTextPosition(5, drawPositionY);
        interfaces.surface->printText(leftText);

        const auto textWidth{ interfaces.surface->getTextSize(font, rightText).first };
        interfaces.surface->setTextPosition(width - textWidth - 5, drawPositionY);
        interfaces.surface->printText(rightText);
    }
}