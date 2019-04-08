#include "../Config.h"
#include "Glow.h"
#include "../Memory.h"
#include "../SDK/ClientClass.h"

void Glow::render() noexcept
{
    constexpr auto& glow = config.glow;

    for (int i = 0; i < memory.glowObjectManager->glowObjectDefinitions.size; i++) {
        GlowObjectDefinition& glowobject = memory.glowObjectManager->glowObjectDefinitions[i];

        auto entity = glowobject.entity;

        if (glowobject.isUnused() || !entity || entity->isDormant())
            continue;

        auto applyGlow = [&glowobject](decltype(glow[0])& glow) noexcept
        {
            glowobject.renderWhenOccluded = glow.enabled;
            if (glow.enabled) {
                glowobject.alpha = glow.alpha;
                glowobject.glowStyle = glow.style;
                glowobject.bloomAmount = glow.thickness;
                glowobject.glowColor = glow.color;
            }
        };

        switch (entity->getClientClass()->classId) {
        case ClassId::CSPlayer: {
            auto activeWeapon = interfaces.entityList->getEntityFromHandle(entity->getProperty<int>("m_hActiveWeapon"));
            if (activeWeapon && activeWeapon->getClientClass()->classId == ClassId::C4 && activeWeapon->getProperty<bool>("m_bStartedArming")) {
                if (entity->isVisible()) applyGlow(glow[4]);
                else applyGlow(glow[5]);
            }
            else if (entity->getProperty<bool>("m_bIsDefusing")) {
                if (entity->isVisible()) applyGlow(glow[6]);
                else applyGlow(glow[7]);
            }
            else if (entity == interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer())) {
                applyGlow(glow[8]);
            }
            else if (entity->isEnemy()) {
                if (entity->isVisible()) applyGlow(glow[2]);
                else applyGlow(glow[3]);
            }
            else {
                if (entity->isVisible()) applyGlow(glow[0]);
                else applyGlow(glow[1]);
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
