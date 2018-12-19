#include <chrono>
#include <Windows.h>

#include "../Config.h"
#include "../Interfaces.h"
#include "../Memory.h"
#include "Triggerbot.h"

void Triggerbot::run(UserCmd* cmd) noexcept
{
    if (config.triggerbot.enabled) {
        static auto lastTime = std::chrono::steady_clock::now();
        if ((GetAsyncKeyState(VK_MENU) || !config.triggerbot.onKey)
            && std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - lastTime).count() >= config.triggerbot.shotDelay) {
            auto inCrosshair = (*memory.localPlayer)->getCrosshairID();
            if (inCrosshair > 0 && inCrosshair <= 64) {
                auto target = interfaces.entityList->getClientEntity(inCrosshair);
                if (target->isEnemy()) {
                    constexpr int IN_ATTACK{ 1 };
                    cmd->buttons |= IN_ATTACK;
                }
            }
            lastTime = std::chrono::steady_clock::now();
        }
    }
}
