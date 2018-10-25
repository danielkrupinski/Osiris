#include "../Config.h"
#include "../Memory.h"
#include "Visuals.h"

void Visuals::glow()
{
    static constexpr auto& glow = config.visuals.glow;

    if (glow.enabled) {
        for (int i = 0; i < memory.glowObjectManager->glowObjectDefinitions.size; i++) {
            GlowObjectDefinition& glowobject = memory.glowObjectManager->glowObjectDefinitions[i];

            if (memory.glowObjectManager->glowObjectDefinitions.elements->isUnused() || !glowobject.entity)
                continue;

            if (glowobject.entity->IsDormant())
                continue;

            glowobject.alpha = glow.alpha;
            glowobject.renderWhenOccluded = true;
            glowobject.renderWhenUnoccluded = false;
            glowobject.glowStyle = glow.style;
            glowobject.fullBloomRender = false;
            glowobject.fullBloomStencilTestValue = 0;
            glowobject.bloomAmount = glow.thickness;

            switch (glowobject.entity->GetClientClass()->m_ClassID) {
            case ClassId_CCSPlayer:
                if (glow.players) {
                    bool isEnemy = reinterpret_cast<BaseEntity*>(glowobject.entity)->getTeamNumber() != (*memory.localPlayer)->getTeamNumber();

                    if (isEnemy) {
                        glowobject.glowColor = glow.enemiesColor;
                    }
                    else if (!glow.enemiesOnly) {
                        glowobject.glowColor = glow.alliesColor;
                    }
                }
                break;
            case ClassId_CChicken:
                if (glow.chickens)
                    glowobject.glowColor = glow.chickensColor;
                else
                    glowobject.renderWhenOccluded = false;
                break;
            default:
                if (glowobject.entity->isWeapon()) {
                    if (glow.weapons)
                        glowobject.glowColor = glow.weaponsColor;
                    else
                        glowobject.renderWhenOccluded = false;
                    break;
                }
                glowobject.renderWhenOccluded = false;
            }
        }
    }
}
