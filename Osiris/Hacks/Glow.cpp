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

        glowobject.renderWhenOccluded = false;
        glowobject.renderWhenUnoccluded = false;
        glowobject.fullBloomRender = false;
        glowobject.fullBloomStencilTestValue = 0;

        switch (entity->getClientClass()->classId) {
        case ClassId::CSPlayer:
            if (entity == interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer())) {
                if (glow[2].enabled) {
                    glowobject.alpha = glow[2].alpha;
                    glowobject.glowStyle = glow[2].style;
                    glowobject.bloomAmount = glow[2].thickness;
                    glowobject.glowColor = glow[2].color;
                    glowobject.renderWhenOccluded = true;
                }
            }
            else if (entity->isEnemy()) {
                if (glow[1].enabled) {
                    glowobject.alpha = glow[1].alpha;
                    glowobject.glowStyle = glow[1].style;
                    glowobject.bloomAmount = glow[1].thickness;
                    glowobject.glowColor = glow[1].color;
                    glowobject.renderWhenOccluded = true;
                }
            }
            else if (glow[0].enabled) {
                glowobject.alpha = glow[0].alpha;
                glowobject.glowStyle = glow[0].style;
                glowobject.bloomAmount = glow[0].thickness;
                glowobject.glowColor = glow[0].color;
                glowobject.renderWhenOccluded = true;
            }
            break;
        case ClassId::Chicken:
            if (glow[6].enabled) {
                glowobject.alpha = glow[6].alpha;
                glowobject.glowStyle = glow[6].style;
                glowobject.bloomAmount = glow[6].thickness;
                glowobject.glowColor = glow[6].color;
                glowobject.renderWhenOccluded = true;
            }
            break;
        case ClassId::C4:
            if (glow[4].enabled) {
                glowobject.alpha = glow[4].alpha;
                glowobject.glowStyle = glow[4].style;
                glowobject.bloomAmount = glow[4].thickness;
                glowobject.glowColor = glow[4].color;
                glowobject.renderWhenOccluded = true;
            }
            break;
        case ClassId::PlantedC4:
            if (glow[5].enabled) {
                glowobject.alpha = glow[5].alpha;
                glowobject.glowStyle = glow[5].style;
                glowobject.bloomAmount = glow[5].thickness;
                glowobject.glowColor = glow[5].color;
                glowobject.renderWhenOccluded = true;
            }
            break;
        default:
            if (entity->isWeapon()) {
                if (glow[3].enabled) {
                    glowobject.alpha = glow[3].alpha;
                    glowobject.glowStyle = glow[3].style;
                    glowobject.bloomAmount = glow[3].thickness;
                    glowobject.glowColor = glow[3].color;
                    glowobject.renderWhenOccluded = true;
                }
            }
        }
    }
}
