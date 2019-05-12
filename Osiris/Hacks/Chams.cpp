#include <fstream>

#include "Chams.h"
#include "../Config.h"
#include "../Hooks.h"
#include "../Interfaces.h"
#include "Backtrack.h"
#include "../SDK/Material.h"
#include "../SDK/MaterialSystem.h"

Chams::Chams() noexcept
{
    std::ofstream{ "csgo/materials/chamsNormal.vmt" } <<
        "VertexLitGeneric { }";

    std::ofstream{ "csgo/materials/chamsFlat.vmt" } <<
        "UnlitGeneric { }";

    normal = interfaces.materialSystem->findMaterial("chamsNormal");
    normal->incrementReferenceCount();
    flat = interfaces.materialSystem->findMaterial("chamsFlat");
    flat->incrementReferenceCount();
    palm = interfaces.materialSystem->findMaterial("models/props/de_dust/hr_dust/foliage/palm_bark_01");
    palm->incrementReferenceCount();
    platinum = interfaces.materialSystem->findMaterial("models/player/ct_fbi/ct_fbi_glass");
    platinum->incrementReferenceCount();
    glass = interfaces.materialSystem->findMaterial("models/inventory_items/cologne_prediction/cologne_prediction_glass");
    glass->incrementReferenceCount();
    crystal = interfaces.materialSystem->findMaterial("models/inventory_items/trophy_majors/crystal_clear");
    crystal->incrementReferenceCount();
    chrome = interfaces.materialSystem->findMaterial("models/gibs/glass/glass");
    chrome->incrementReferenceCount();
    gold = interfaces.materialSystem->findMaterial("models/inventory_items/trophy_majors/gold");
    gold->incrementReferenceCount();
    plastic = interfaces.materialSystem->findMaterial("models/inventory_items/trophy_majors/gloss");
    plastic->incrementReferenceCount();
    blinking = interfaces.materialSystem->findMaterial("models/inventory_items/dogtags/dogtags_outline");
    blinking->incrementReferenceCount();
}

void Chams::render(void* ctx, void* state, const ModelRenderInfo& info, matrix3x4* customBoneToWorld) const noexcept
{
    const auto isLocalPlayerAlive = interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer())->isAlive();
    if (strstr(info.model->name, "models/player"))
        renderPlayers(ctx, state, info, customBoneToWorld);
    else if (isLocalPlayerAlive && strstr(info.model->name, "arms"))
        renderHands();
    else if (isLocalPlayerAlive && strstr(info.model->name, "models/weapons/v_")
        && !strstr(info.model->name, "tablet")
        && !strstr(info.model->name, "parachute")
        && !strstr(info.model->name, "fists"))
        renderWeapons();
}

void Chams::renderPlayers(void* ctx, void* state, const ModelRenderInfo& info, matrix3x4* customBoneToWorld) const noexcept
{
    auto entity = interfaces.entityList->getEntity(info.entityIndex);

    if (entity && !entity->isDormant() && entity->isAlive()) {
        auto activeWeapon = interfaces.entityList->getEntityFromHandle(entity->getProperty<int>("m_hActiveWeapon"));
        if (activeWeapon && activeWeapon->getClientClass()->classId == ClassId::C4 && activeWeapon->getProperty<bool>("m_bStartedArming")) {
            if (config.chams[PLANTING_OCCLUDED].enabled) {
                applyChams(config.chams[PLANTING_OCCLUDED], true, entity->getProperty<int>("m_iHealth"));
                hooks.modelRender.callOriginal<void, void*, void*, const ModelRenderInfo&, matrix3x4*>(21, ctx, state, info, customBoneToWorld);
                if (!config.chams[PLANTING_VISIBLE].enabled)
                    interfaces.modelRender->forceMaterialOverride(nullptr);
            }
            if (config.chams[PLANTING_VISIBLE].enabled)
                applyChams(config.chams[PLANTING_VISIBLE], false, entity->getProperty<int>("m_iHealth"));
        } else if (entity->getProperty<bool>("m_bIsDefusing")) {
            if (config.chams[DEFUSING_OCCLUDED].enabled) {
                applyChams(config.chams[DEFUSING_OCCLUDED], true, entity->getProperty<int>("m_iHealth"));
                hooks.modelRender.callOriginal<void, void*, void*, const ModelRenderInfo&, matrix3x4*>(21, ctx, state, info, customBoneToWorld);
                if (!config.chams[DEFUSING_VISIBLE].enabled)
                    interfaces.modelRender->forceMaterialOverride(nullptr);
            }
            if (config.chams[DEFUSING_VISIBLE].enabled)
                applyChams(config.chams[DEFUSING_VISIBLE], false, entity->getProperty<int>("m_iHealth"));
        } else if (info.entityIndex == interfaces.engine->getLocalPlayer()) {
            if (config.chams[LOCALPLAYER].enabled)
                applyChams(config.chams[LOCALPLAYER], false, entity->getProperty<int>("m_iHealth"));
        } else if (entity->isEnemy()) {
            if (config.chams[ENEMIES_OCCLUDED].enabled) {
                applyChams(config.chams[ENEMIES_OCCLUDED], true, entity->getProperty<int>("m_iHealth"));
                hooks.modelRender.callOriginal<void, void*, void*, const ModelRenderInfo&, matrix3x4*>(21, ctx, state, info, customBoneToWorld);
                if (!config.chams[ENEMIES_VISIBLE].enabled)
                    interfaces.modelRender->forceMaterialOverride(nullptr);
            }
            if (config.chams[BACKTRACK].enabled && config.backtrack.enabled) {
                auto record = &Backtrack::records[info.entityIndex];
                if (record && record->size() && Backtrack::valid(record->front().simulationTime)) {
                    applyChams(config.chams[BACKTRACK], false, entity->getProperty<int>("m_iHealth"));
                    hooks.modelRender.callOriginal<void, void*, void*, const ModelRenderInfo&, matrix3x4*>(21, ctx, state, info, record->back().matrix);
                    interfaces.modelRender->forceMaterialOverride(nullptr);
                }
            }
            if (config.chams[ENEMIES_VISIBLE].enabled)
                applyChams(config.chams[ENEMIES_VISIBLE], false, entity->getProperty<int>("m_iHealth"));
        } else {
            if (config.chams[ALLIES_OCCLUDED].enabled) {
                applyChams(config.chams[ALLIES_OCCLUDED], true, entity->getProperty<int>("m_iHealth"));
                hooks.modelRender.callOriginal<void, void*, void*, const ModelRenderInfo&, matrix3x4*>(21, ctx, state, info, customBoneToWorld);
                if (!config.chams[ALLIES_VISIBLE].enabled)
                    interfaces.modelRender->forceMaterialOverride(nullptr);
            }
            if (config.chams[ALLIES_VISIBLE].enabled)
                applyChams(config.chams[ALLIES_VISIBLE], false, entity->getProperty<int>("m_iHealth"));
        }
    }
}
