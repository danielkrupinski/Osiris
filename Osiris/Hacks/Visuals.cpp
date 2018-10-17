#include "../Config.h"
#include "../Memory.h"
#include "Visuals.h"

void Visuals::glow()
{
    if (config.visuals.glow.enabled) {
        for (int index = 0; index < memory.glowObjectManager->m_GlowObjectDefinitions.m_Size; index++) {
            GlowObjectDefinition_t& glowobject = memory.glowObjectManager->m_GlowObjectDefinitions[index];

            if (memory.glowObjectManager->m_GlowObjectDefinitions.m_pElements->IsUnused() || !glowobject.m_pEntity)
                continue;

            if (glowobject.m_pEntity->GetClientClass()->m_ClassID != ClassId_CCSPlayer || glowobject.m_pEntity->IsDormant())
                continue;
            //entity->setShouldGlow(true);

            glowobject.m_vGlowColor = Vector{ 153.0f, 0.0f, 0.0f };
            glowobject.m_flAlpha = 1.0f;
            glowobject.m_bRenderWhenOccluded = true;
            glowobject.m_bRenderWhenUnoccluded = false;
            glowobject.m_nGlowStyle = 0;
            glowobject.m_bFullBloomRender = false;
            glowobject.m_nFullBloomStencilTestValue = 0;
            glowobject.m_flBloomAmount = 0.5f;
        }
    }
}
