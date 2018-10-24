#include "../Config.h"
#include "../Memory.h"
#include "Visuals.h"

void Visuals::glow()
{
    static constexpr auto& glow = config.visuals.glow;

    if (glow.enabled) {
        for (int index = 0; index < memory.glowObjectManager->glowObjectDefinitions.m_Size; index++) {
            GlowObjectDefinition& glowobject = memory.glowObjectManager->glowObjectDefinitions[index];

            if (memory.glowObjectManager->glowObjectDefinitions.m_pElements->isUnused() || !glowobject.entity)
                continue;

            if (glowobject.entity->IsDormant())
                continue;

            if (glowobject.entity->GetClientClass()->m_ClassID == ClassId_CCSPlayer) {
                if (glow.players) {
                    bool isEnemy = reinterpret_cast<BaseEntity*>(glowobject.entity)->getTeamNumber() != (*memory.localPlayer)->getTeamNumber();

                    if (isEnemy) {
                        glowobject.glowColor = glow.enemiesColor;
                        glowobject.alpha = glow.alpha;
                        glowobject.renderWhenOccluded = true;
                        glowobject.renderWhenUnoccluded = false;
                        glowobject.glowStyle = glow.style;
                        glowobject.fullBloomRender = false;
                        glowobject.fullBloomStencilTestValue = 0;
                        glowobject.bloomAmount = glow.thickness;
                    }
                    else if (!glow.enemiesOnly) {
                        glowobject.glowColor = glow.alliesColor;
                        glowobject.alpha = glow.alpha;
                        glowobject.renderWhenOccluded = true;
                        glowobject.renderWhenUnoccluded = false;
                        glowobject.glowStyle = glow.style;
                        glowobject.fullBloomRender = false;
                        glowobject.fullBloomStencilTestValue = 0;
                        glowobject.bloomAmount = glow.thickness;
                    }
                }
                    

            }
            else if (glowobject.entity->isWeapon()) {
                if (glow.weapons) {
                    glowobject.glowColor = glow.weaponsColor;
                    glowobject.alpha = glow.alpha;
                    glowobject.renderWhenOccluded = true;
                    glowobject.renderWhenUnoccluded = false;
                    glowobject.glowStyle = glow.style;
                    glowobject.fullBloomRender = false;
                    glowobject.fullBloomStencilTestValue = 0;
                    glowobject.bloomAmount = glow.thickness;
                }
            }
            else if (glowobject.entity->GetClientClass()->m_ClassID == ClassId_CChicken) {
                glowobject.glowColor = glow.chickensColor;
                glowobject.alpha = glow.alpha;
                glowobject.renderWhenOccluded = true;
                glowobject.renderWhenUnoccluded = false;
                glowobject.glowStyle = glow.style;
                glowobject.fullBloomRender = false;
                glowobject.fullBloomStencilTestValue = 0;
                glowobject.bloomAmount = glow.thickness;
            }
        }
    }
}
