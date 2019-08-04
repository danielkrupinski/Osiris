#include "../Config.h"
#include "Glow.h"
#include "../Memory.h"
#include "../SDK/ClientClass.h"
#include "../SDK/GlowObjectManager.h"
#include "../SDK/GlobalVars.h"

void Glow::render() noexcept
{
    constexpr auto& glow = config.glow;

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
                else if (glow.rainbow)
                    glowobject.glowColor = { sinf(0.6f * memory.globalVars->currenttime) * 0.5f + 0.5f,
                                             sinf(0.6f * memory.globalVars->currenttime + 2.0f) * 0.5f + 0.5f,
                                             sinf(0.6f * memory.globalVars->currenttime + 4.0f) * 0.5f + 0.5f };
                else
                    glowobject.glowColor = glow.color;
            }
        };

        const auto localPlayer = interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer());

        auto applyPlayerGlow = [applyGlow, localPlayer](decltype(glow[0])& glowAll, decltype(glow[0])& glowVisible, decltype(glow[0])& glowOccluded, Entity* entity) noexcept {
            if (glowAll.enabled) applyGlow(glowAll, entity->health());
            else if (entity->isVisible() && !memory.lineGoesThroughSmoke(localPlayer->getEyePosition(), entity->getBonePosition(8), 1)) applyGlow(glowVisible, entity->health());
            else applyGlow(glowOccluded, entity->health());
        };

        switch (entity->getClientClass()->classId) {
        case ClassId::CSPlayer:
            if (auto activeWeapon{ entity->getActiveWeapon() }; activeWeapon && activeWeapon->getClientClass()->classId == ClassId::C4 && activeWeapon->getProperty<bool>("m_bStartedArming"))
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
        case ClassId::Chicken: applyGlow(glow[16]); break;
        case ClassId::C4: applyGlow(glow[14]); break;
        case ClassId::PlantedC4: applyGlow(glow[15]); break;
        default:
            if (entity->isWeapon()) {
                applyGlow(glow[13]);
                if (!glow[13].enabled) glowobject.renderWhenOccluded = false;
            }
        }
    }
}
