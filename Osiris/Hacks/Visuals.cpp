#include "../Config.h"
#include "../Memory.h"
#include "Visuals.h"

void Visuals::glow()
{
    if (config.visuals.glow.enabled) {
        for (int index = 0; index < memory.glowObjectManager->m_GlowObjectDefinitions.m_Size; index++) {
            GlowObjectDefinition_t& glowobject = memory.glowObjectManager->m_GlowObjectDefinitions[index];

            if (memory.glowObjectManager->m_GlowObjectDefinitions.m_pElements->isUnused() || !glowobject.m_pEntity)
                continue;

            if (glowobject.m_pEntity->GetClientClass()->m_ClassID != ClassId_CCSPlayer || glowobject.m_pEntity->IsDormant())
                continue;

            glowobject.m_vGlowColor = config.visuals.glow.color;
            glowobject.m_flAlpha = config.visuals.glow.alpha;
            glowobject.m_bRenderWhenOccluded = true;
            glowobject.m_bRenderWhenUnoccluded = false;
            glowobject.m_nGlowStyle = 0;
            glowobject.m_bFullBloomRender = false;
            glowobject.m_nFullBloomStencilTestValue = 0;
            glowobject.m_flBloomAmount = config.visuals.glow.thickness;
        }
    }
}
