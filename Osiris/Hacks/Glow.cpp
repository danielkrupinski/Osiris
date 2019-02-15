#include "../Config.h"
#include "Glow.h"
#include "../Memory.h"
#include "../SDK/ClientClass.h"

void Glow::render() noexcept
{
    constexpr auto& glow = config.glow;

    for (int i = 0; i < memory.glowObjectManager->glowObjectDefinitions.size; i++) {
        GlowObjectDefinition& glowobject = memory.glowObjectManager->glowObjectDefinitions[i];

        if (glowobject.isUnused() || !glowobject.entity)
            continue;

        if (glowobject.entity->isDormant())
            continue;

        glowobject.renderWhenOccluded = false;

        if (glow.enabled) {

            glowobject.alpha = glow.alpha;
            glowobject.renderWhenUnoccluded = false;
            glowobject.glowStyle = glow.style;
            glowobject.fullBloomRender = false;
            glowobject.fullBloomStencilTestValue = 0;
            glowobject.bloomAmount = glow.thickness;

            switch (glowobject.entity->getClientClass()->classId) {
            case ClassId::CSPlayer:
                if (glowobject.entity == interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer())) {
                    if (glow.localPlayer) {
                        glowobject.glowColor = glow.localPlayerColor;
                        glowobject.renderWhenOccluded = true;
                    }
                }
                else if (glowobject.entity->isEnemy()) {
                    if (glow.enemies) {
                        glowobject.glowColor = glow.enemiesColor;
                        glowobject.renderWhenOccluded = true;
                    }
                }
                else if (glow.allies) {
                    glowobject.glowColor = glow.alliesColor;
                    glowobject.renderWhenOccluded = true;
                }
                break;
            case ClassId::Chicken:
                if (glow.chickens) {
                    glowobject.glowColor = glow.chickensColor;
                    glowobject.renderWhenOccluded = true;
                }
                break;
            case ClassId::C4:
                if (glow.C4) {
                    glowobject.glowColor = glow.C4Color;
                    glowobject.renderWhenOccluded = true;
                }
                break;
            case ClassId::PlantedC4:
                if (glow.plantedC4) {
                    glowobject.glowColor = glow.plantedC4Color;
                    glowobject.renderWhenOccluded = true;
                }
                break;
            default:
                if (glowobject.entity->isWeapon()) {
                    if (glow.weapons) {
                        glowobject.glowColor = glow.weaponsColor;
                        glowobject.renderWhenOccluded = true;
                    }
                }
            }
        }
    }
}
