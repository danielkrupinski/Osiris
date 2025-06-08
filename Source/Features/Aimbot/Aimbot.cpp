#include "Aimbot.h"

#include <cmath>
#include <vector>
#include <algorithm>
#include <iostream>

#include "../../Config/ConfigVariables.h"
#include "../../HookContext/HookContext.h"
#include "../../CS2/Classes/Vector.h"
#include "../../GameClient/Entities/PlayerController.h"
#include "../../GameClient/Entities/PlayerPawn.h"
#include "../../GameClient/EntitySystem/EntitySystem.h"


#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

// Helper to convert radians to degrees
static constexpr float rad2deg(float rad) {
    return rad * (180.0f / M_PI);
}

// Helper to normalize yaw angle to [-180, 180]
static float normalizeYaw(float yaw) {
    float result = yaw;
    while (result > 180.0f) result -= 360.0f;
    while (result < -180.0f) result += 360.0f;
    return result;
}

PlayerPawn Aimbot::findTarget(HookContext& hookContext, const Vector& localPlayerEyePosition) {
    PlayerPawn bestTargetPawn{nullptr};

    AimbotConfigVariables aimbotSettings;
    float minMetric = aimbotSettings.aimbotFov;

    const auto& entitySystem = hookContext.entitySystem();
    if (!entitySystem) {
        std::cerr << "Aimbot::findTarget: EntitySystem is null" << std::endl;
        return bestTargetPawn;
    }

    entitySystem.forEachEntityIdentity([&](auto entityIdentity) {
        auto entity = entityIdentity.entity();
        if (!entity || !entity.isPlayerPawn()) {
            return;
        }

        PlayerPawn playerPawn = entity.template as<PlayerPawn>();
        if (!playerPawn.isAlive() || !playerPawn.isEnemy(hookContext.localPlayerController())) {
            return;
        }

        Vector enemyOrigin = playerPawn.absOrigin();

        float currentMetric = localPlayerEyePosition.distTo(enemyOrigin);

        if (currentMetric < minMetric) {
            minMetric = currentMetric;
            bestTargetPawn = playerPawn;
        }
    });

    if (bestTargetPawn) {
        // std::cout << "Aimbot::findTarget: Found target!" << std::endl;
    }
    return bestTargetPawn;
}

void Aimbot::calculateAimAngles(const Vector& localPlayerEyePosition, const Vector& targetPosition, Vector& outAimAngles) {
    Vector delta = targetPosition - localPlayerEyePosition;

    outAimAngles.x = rad2deg(atan2(-delta.z, std::sqrt(delta.x * delta.x + delta.y * delta.y)));
    outAimAngles.y = normalizeYaw(rad2deg(atan2(delta.y, delta.x)));
    outAimAngles.z = 0.0f;

    // std::cout << "Aimbot::calculateAimAngles: Pitch: " << outAimAngles.x << " Yaw: " << outAimAngles.y << std::endl;
}

void Aimbot::run(HookContext& hookContext) {
    AimbotConfigVariables aimbotSettings;

    if (!aimbotSettings.aimbotEnabled) {
        return;
    }

    auto localPlayerController = hookContext.localPlayerController();
    if (!localPlayerController) {
        // std::cerr << "Aimbot::run: LocalPlayerController is null" << std::endl;
        return;
    }

    auto localPlayerPawn = localPlayerController.playerPawn();
    if (!localPlayerPawn || !localPlayerPawn.isAlive()) {
        // std::cerr << "Aimbot::run: LocalPlayerPawn is null or not alive" << std::endl;
        return;
    }

    Vector localPlayerEyePosition = localPlayerPawn.absOrigin() + Vector{0.0f, 0.0f, 64.0f};

    PlayerPawn targetPawn = findTarget(hookContext, localPlayerEyePosition);

    if (targetPawn) {
        Vector targetPosition = targetPawn.absOrigin();

        calculateAimAngles(localPlayerEyePosition, targetPosition, this->currentAimAngles);

        std::cout << "Aimbot::run: Calculated aim angles - Pitch: " << this->currentAimAngles.x << " Yaw: " << this->currentAimAngles.y << std::endl;
    } else {
        // std::cout << "Aimbot::run: No target found." << std::endl;
    }
}
