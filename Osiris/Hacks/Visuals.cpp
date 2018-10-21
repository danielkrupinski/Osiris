#include "../Config.h"
#include "../Memory.h"
#include "Visuals.h"

void Visuals::glow()
{
    static constexpr auto& glow = config.visuals.glow;

    if (glow.enabled) {
        for (int index = 0; index < memory.glowObjectManager->m_GlowObjectDefinitions.m_Size; index++) {
            GlowObjectDefinition& glowobject = memory.glowObjectManager->m_GlowObjectDefinitions[index];

            if (memory.glowObjectManager->m_GlowObjectDefinitions.m_pElements->isUnused() || !glowobject.entity)
                continue;

            if (glowobject.entity->GetClientClass()->m_ClassID != ClassId_CCSPlayer || glowobject.entity->IsDormant())
                continue;

            bool isEnemy = reinterpret_cast<BaseEntity*>(glowobject.entity)->getTeamNumber() != (*memory.localPlayer)->getTeamNumber();

            if (isEnemy) {
                glowobject.glowColor = glow.enemiesColor;
                glowobject.alpha = glow.alpha;
                glowobject.renderWhenOccluded = true;
                glowobject.renderWhenUnoccluded = false;
                glowobject.glowStyle = 0;
                glowobject.fullBloomRender = false;
                glowobject.fullBloomStencilTestValue = 0;
                glowobject.bloomAmount = glow.thickness;
            }
            else if (!glow.enemiesOnly) {
                glowobject.glowColor = glow.alliesColor;
                glowobject.alpha = glow.alpha;
                glowobject.renderWhenOccluded = true;
                glowobject.renderWhenUnoccluded = false;
                glowobject.glowStyle = 0;
                glowobject.fullBloomRender = false;
                glowobject.fullBloomStencilTestValue = 0;
                glowobject.bloomAmount = glow.thickness;
            }
        }
    }
}
