#include "../Config.h"
#include "Glow.h"
#include "../Interfaces.h"
#include "../Memory.h"
#include "../SDK/Entity.h"
#include "../SDK/ClientClass.h"
#include "../SDK/GlowObjectManager.h"
#include "../SDK/GlobalVars.h"
#include "../SDK/Utils.h"

static std::vector<std::pair<int, int>> customGlowEntities;

void Glow::render() noexcept
{
    const auto localPlayer = interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer());

    if (!localPlayer)
        return;

    constexpr auto& glow = config.glow;

    Glow::clearCustomObjects();

    for (int i = interfaces.engine->getMaxClients() + 1; i <= interfaces.entityList->getHighestEntityIndex(); ++i) {
        const auto entity = interfaces.entityList->getEntity(i);
        if (!entity || entity->isDormant())
            continue;

        switch (entity->getClientClass()->classId) {
        case ClassId::EconEntity:
        case ClassId::BaseCSGrenadeProjectile:
        case ClassId::BreachChargeProjectile:
        case ClassId::BumpMineProjectile:
        case ClassId::DecoyProjectile:
        case ClassId::MolotovProjectile:
        case ClassId::SensorGrenadeProjectile:
        case ClassId::SmokeGrenadeProjectile:
        case ClassId::SnowballProjectile:
        case ClassId::Hostage:
        case ClassId::CSRagdoll:
            if (!memory.glowObjectManager->hasGlowEffect(entity)) {
                if (auto index{ memory.glowObjectManager->registerGlowObject(entity) }; index != -1)
                    customGlowEntities.emplace_back(i, index);
            }
        }
    }

    for (int i = 0; i < memory.glowObjectManager->glowObjectDefinitions.size; i++) {
        GlowObjectDefinition& glowobject = memory.glowObjectManager->glowObjectDefinitions[i];

        auto entity = glowobject.entity;

        if (glowobject.isUnused() || !entity || entity->isDormant())
            continue;

        auto applyGlow = [&glowobject](decltype(glow[0])& glow, int health = 0) noexcept
        {
            if (glow.enabled) {
                glowobject.renderWhenOccluded = true;
                glowobject.alpha = glow.alpha;
                glowobject.glowStyle = glow.style;
                glowobject.bloomAmount = glow.thickness;
                if (glow.healthBased && health)
                    glowobject.glowColor = { 1.0f - health / 100.0f,  health / 100.0f, 0.0f };
                else if (glow.color.rainbow) {
                    const auto [r, g, b] { rainbowColor(memory.globalVars->realtime, glow.color.rainbowSpeed) };
                    glowobject.glowColor = { r, g, b };
                }
                else
                    glowobject.glowColor = glow.color.color;
            }
        };

        auto applyPlayerGlow = [applyGlow, localPlayer](decltype(glow[0])& glowAll, decltype(glow[0])& glowVisible, decltype(glow[0])& glowOccluded, Entity* entity) noexcept {
            if (glowAll.enabled) applyGlow(glowAll, entity->health());
            else if (entity->isVisible() && !memory.lineGoesThroughSmoke(localPlayer->getEyePosition(), entity->getBonePosition(8), 1)) applyGlow(glowVisible, entity->health());
            else applyGlow(glowOccluded, entity->health());
        };

        switch (entity->getClientClass()->classId) {
        case ClassId::CSPlayer:
            if (auto activeWeapon{ entity->getActiveWeapon() }; activeWeapon && activeWeapon->getClientClass()->classId == ClassId::C4 && activeWeapon->c4StartedArming())
                applyPlayerGlow(glow[6], glow[7], glow[8], entity);
            else if (entity->isDefusing())
                applyPlayerGlow(glow[9], glow[10], glow[11], entity);
            else if (entity == interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer()))
                applyGlow(glow[12], entity->health());
            else if (entity->isEnemy())
                applyPlayerGlow(glow[3], glow[4], glow[5], entity);
            else
                applyPlayerGlow(glow[0], glow[1], glow[2], entity);
            break;
        case ClassId::C4: applyGlow(glow[14]); break;
        case ClassId::PlantedC4: applyGlow(glow[15]); break;
        case ClassId::Chicken: applyGlow(glow[16]); break;
        case ClassId::EconEntity: applyGlow(glow[17]); break;

        case ClassId::BaseCSGrenadeProjectile:
        case ClassId::BreachChargeProjectile:
        case ClassId::BumpMineProjectile:
        case ClassId::DecoyProjectile:
        case ClassId::MolotovProjectile:
        case ClassId::SensorGrenadeProjectile:
        case ClassId::SmokeGrenadeProjectile:
        case ClassId::SnowballProjectile:
            applyGlow(glow[18]); break;

        case ClassId::Hostage: applyGlow(glow[19]); break;
        case ClassId::CSRagdoll: applyGlow(glow[20]); break;
        default:
           if (entity->isWeapon()) {
                applyGlow(glow[13]);
                if (!glow[13].enabled) glowobject.renderWhenOccluded = false;
            }
        }
    }
}

void Glow::clearCustomObjects() noexcept
{
    for (const auto& [entityIndex, glowObjectIndex] : customGlowEntities)
        memory.glowObjectManager->unregisterGlowObject(glowObjectIndex);

    customGlowEntities.clear();
}
