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
            glowobject.renderWhenOccluded = glow.enabled;
            if (glow.enabled) {
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

        switch (entity->getClientClass()->classId) {
        case ClassId::CSPlayer: {
            auto activeWeapon = interfaces.entityList->getEntityFromHandle(entity->getProperty<int>("m_hActiveWeapon"));
            if (activeWeapon && activeWeapon->getClientClass()->classId == ClassId::C4 && activeWeapon->getProperty<bool>("m_bStartedArming")) {
                if (entity->isVisible()) applyGlow(glow[4], entity->getProperty<int>("m_iHealth"));
                else applyGlow(glow[5], entity->getProperty<int>("m_iHealth"));
            } else if (entity->getProperty<bool>("m_bIsDefusing")) {
                if (entity->isVisible()) applyGlow(glow[6], entity->getProperty<int>("m_iHealth"));
                else applyGlow(glow[7], entity->getProperty<int>("m_iHealth"));
            } else if (entity == interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer())) {
                applyGlow(glow[8], entity->getProperty<int>("m_iHealth"));
            } else if (entity->isEnemy()) {
                if (entity->isVisible()) applyGlow(glow[2], entity->getProperty<int>("m_iHealth"));
                else applyGlow(glow[3], entity->getProperty<int>("m_iHealth"));
            } else {
                if (entity->isVisible()) applyGlow(glow[0], entity->getProperty<int>("m_iHealth"));
                else applyGlow(glow[1], entity->getProperty<int>("m_iHealth"));
            }
            break;
        }
        case ClassId::Chicken: applyGlow(glow[12]); break;
        case ClassId::C4: applyGlow(glow[10]); break;
        case ClassId::PlantedC4: applyGlow(glow[11]); break;
        default: if (entity->isWeapon()) applyGlow(glow[9]);
        }
    }
}
