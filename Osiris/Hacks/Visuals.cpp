#include "../Config.h"
#include "../Memory.h"
#include "Visuals.h"

void Visuals::glow()
{
    if (config.visuals.glow.enabled) {
        for (int index = 0; index < memory.glowObjectManager->m_GlowObjectDefinitions.m_Size; index++) {
            GlowObjectDefinition& glowobject = memory.glowObjectManager->m_GlowObjectDefinitions[index];

            if (memory.glowObjectManager->m_GlowObjectDefinitions.m_pElements->isUnused() || !glowobject.m_pEntity)
                continue;

            if (glowobject.m_pEntity->GetClientClass()->m_ClassID != ClassId_CCSPlayer || glowobject.m_pEntity->IsDormant())
                continue;

            bool isEnemy = reinterpret_cast<BaseEntity*>(glowobject.m_pEntity)->getTeamNumber() != (*memory.localPlayer)->getTeamNumber();

            const auto& glow = config.visuals.glow;

            if (isEnemy) {
                glowobject.m_vGlowColor = glow.enemiesColor;
                glowobject.m_flAlpha = glow.alpha;
                glowobject.m_bRenderWhenOccluded = true;
                glowobject.m_bRenderWhenUnoccluded = false;
                glowobject.m_nGlowStyle = 0;
                glowobject.m_bFullBloomRender = false;
                glowobject.m_nFullBloomStencilTestValue = 0;
                glowobject.m_flBloomAmount = glow.thickness;
            }
            else if (!glow.enemiesOnly) {
                glowobject.m_vGlowColor = { 1.0f, 1.0f, 1.0f };
                glowobject.m_flAlpha = glow.alpha;
                glowobject.m_bRenderWhenOccluded = true;
                glowobject.m_bRenderWhenUnoccluded = false;
                glowobject.m_nGlowStyle = 0;
                glowobject.m_bFullBloomRender = false;
                glowobject.m_nFullBloomStencilTestValue = 0;
                glowobject.m_flBloomAmount = glow.thickness;
            }
        }
    }
}
