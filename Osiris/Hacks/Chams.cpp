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

    std::ofstream{ "csgo/materials/chamsAnimated.vmt" } <<
        "VertexLitGeneric { $envmap editor/cube_vertigo $envmapcontrast 1 $envmaptint \"[.7 .7 .7]\" $basetexture dev/zone_warning proxies { texturescroll { texturescrollvar $basetexturetransform texturescrollrate 0.6 texturescrollangle 90 } } }";

    normal = interfaces.materialSystem->findMaterial("chamsNormal");
    normal->incrementReferenceCount();
    flat = interfaces.materialSystem->findMaterial("chamsFlat");
    flat->incrementReferenceCount();
    animated = interfaces.materialSystem->findMaterial("chamsAnimated");
    animated->incrementReferenceCount();
    platinum = interfaces.materialSystem->findMaterial("models/player/ct_fbi/ct_fbi_glass");
    platinum->incrementReferenceCount();
    glass = interfaces.materialSystem->findMaterial("models/inventory_items/cologne_prediction/cologne_prediction_glass");
    glass->incrementReferenceCount();
    crystal = interfaces.materialSystem->findMaterial("models/inventory_items/trophy_majors/crystal_clear");
    crystal->incrementReferenceCount();
    chrome = interfaces.materialSystem->findMaterial("models/gibs/glass/glass");
    chrome->incrementReferenceCount();
    silver = interfaces.materialSystem->findMaterial("models/inventory_items/trophy_majors/silver_plain");
    silver->incrementReferenceCount();
    gold = interfaces.materialSystem->findMaterial("models/inventory_items/trophy_majors/gold");
    gold->incrementReferenceCount();
    plastic = interfaces.materialSystem->findMaterial("models/inventory_items/trophy_majors/gloss");
    plastic->incrementReferenceCount();
}

bool Chams::render(void* ctx, void* state, const ModelRenderInfo& info, matrix3x4* customBoneToWorld) const noexcept
{
    const auto isLocalPlayerAlive = interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer())->isAlive();
    if (strstr(info.model->name, "models/player"))
        return renderPlayers(ctx, state, info, customBoneToWorld);
    else if (isLocalPlayerAlive && strstr(info.model->name, "sleeve"))
        renderSleeves(ctx, state, info, customBoneToWorld);
    else if (isLocalPlayerAlive && strstr(info.model->name, "arms"))
        renderHands(ctx, state, info, customBoneToWorld);
    else if (isLocalPlayerAlive && strstr(info.model->name, "models/weapons/v_")
        && !strstr(info.model->name, "tablet")
        && !strstr(info.model->name, "parachute")
        && !strstr(info.model->name, "fists"))
        renderWeapons(ctx, state, info, customBoneToWorld);
    return true;
}

bool Chams::renderPlayers(void* ctx, void* state, const ModelRenderInfo& info, matrix3x4* customBoneToWorld) const noexcept
{
    if (interfaces.modelRender->isMaterialOverridden())
        return true;

    bool needRedraw = true;

    if (auto entity{ interfaces.entityList->getEntity(info.entityIndex) }; entity && !entity->isDormant() && entity->isAlive()) {
        auto applied{ false };
        for (size_t i = 0; i < config.chams[ALLIES_ALL].materials.size(); i++) {
            if (auto activeWeapon{ entity->getActiveWeapon() }; activeWeapon && activeWeapon->getClientClass()->classId == ClassId::C4 && activeWeapon->c4StartedArming()) {
                if (config.chams[PLANTING_ALL].materials[i].enabled) {
                    if (applied)
                        hooks.modelRender.callOriginal<void, void*, void*, const ModelRenderInfo&, matrix3x4*>(21, ctx, state, info, customBoneToWorld);
                    applyChams(config.chams[PLANTING_ALL].materials[i], true, entity->health());
                    hooks.modelRender.callOriginal<void, void*, void*, const ModelRenderInfo&, matrix3x4*>(21, ctx, state, info, customBoneToWorld);
                    applyChams(config.chams[PLANTING_ALL].materials[i], false, entity->health());
                    applied = true;
                } else {
                    if (config.chams[PLANTING_OCCLUDED].materials[i].enabled) {
                        if (applied)
                            hooks.modelRender.callOriginal<void, void*, void*, const ModelRenderInfo&, matrix3x4*>(21, ctx, state, info, customBoneToWorld);
                        applyChams(config.chams[PLANTING_OCCLUDED].materials[i], true, entity->health());
                        hooks.modelRender.callOriginal<void, void*, void*, const ModelRenderInfo&, matrix3x4*>(21, ctx, state, info, customBoneToWorld);
                        if (!config.chams[PLANTING_VISIBLE].materials[i].enabled)
                            interfaces.modelRender->forceMaterialOverride(nullptr);
                        applied = true;
                    }
                    if (config.chams[PLANTING_VISIBLE].materials[i].enabled) {
                        if (applied)
                            hooks.modelRender.callOriginal<void, void*, void*, const ModelRenderInfo&, matrix3x4*>(21, ctx, state, info, customBoneToWorld);
                        applyChams(config.chams[PLANTING_VISIBLE].materials[i], false, entity->health());
                        applied = true;
                    }
                }
            } else if (entity->isDefusing()) {
                if (config.chams[DEFUSING_ALL].materials[i].enabled) {
                    if (applied)
                        hooks.modelRender.callOriginal<void, void*, void*, const ModelRenderInfo&, matrix3x4*>(21, ctx, state, info, customBoneToWorld);
                    applyChams(config.chams[DEFUSING_ALL].materials[i], true, entity->health());
                    hooks.modelRender.callOriginal<void, void*, void*, const ModelRenderInfo&, matrix3x4*>(21, ctx, state, info, customBoneToWorld);
                    applyChams(config.chams[DEFUSING_ALL].materials[i], false, entity->health());
                    applied = true;
                } else {
                    if (config.chams[DEFUSING_OCCLUDED].materials[i].enabled) {
                        if (applied)
                            hooks.modelRender.callOriginal<void, void*, void*, const ModelRenderInfo&, matrix3x4*>(21, ctx, state, info, customBoneToWorld);
                        applyChams(config.chams[DEFUSING_OCCLUDED].materials[i], true, entity->health());
                        hooks.modelRender.callOriginal<void, void*, void*, const ModelRenderInfo&, matrix3x4*>(21, ctx, state, info, customBoneToWorld);
                        if (!config.chams[DEFUSING_VISIBLE].materials[i].enabled)
                            interfaces.modelRender->forceMaterialOverride(nullptr);
                        applied = true;
                    }
                    if (config.chams[DEFUSING_VISIBLE].materials[i].enabled) {
                        if (applied)
                            hooks.modelRender.callOriginal<void, void*, void*, const ModelRenderInfo&, matrix3x4*>(21, ctx, state, info, customBoneToWorld);
                        applyChams(config.chams[DEFUSING_VISIBLE].materials[i], false, entity->health());
                        applied = true;
                    }
                }
            } else if (info.entityIndex == interfaces.engine->getLocalPlayer()) {
                if (config.chams[LOCALPLAYER].materials[i].enabled) {
                    if (applied)
                        hooks.modelRender.callOriginal<void, void*, void*, const ModelRenderInfo&, matrix3x4*>(21, ctx, state, info, customBoneToWorld);
                    applyChams(config.chams[LOCALPLAYER].materials[i], false, entity->health());
                    applied = true;
                }
            } else if (entity->isEnemy()) {
                if (config.chams[ENEMIES_ALL].materials[i].enabled) {
                    if (applied)
                        hooks.modelRender.callOriginal<void, void*, void*, const ModelRenderInfo&, matrix3x4*>(21, ctx, state, info, customBoneToWorld);
                    applyChams(config.chams[ENEMIES_ALL].materials[i], true, entity->health());
                    hooks.modelRender.callOriginal<void, void*, void*, const ModelRenderInfo&, matrix3x4*>(21, ctx, state, info, customBoneToWorld);
                    applyChams(config.chams[ENEMIES_ALL].materials[i], false, entity->health());
                    hooks.modelRender.callOriginal<void, void*, void*, const ModelRenderInfo&, matrix3x4*>(21, ctx, state, info, customBoneToWorld);
                    needRedraw = false;
                    applied = true;
                } else {
                    if (config.chams[ENEMIES_OCCLUDED].materials[i].enabled) {
                        if (applied)
                            hooks.modelRender.callOriginal<void, void*, void*, const ModelRenderInfo&, matrix3x4*>(21, ctx, state, info, customBoneToWorld);
                        applyChams(config.chams[ENEMIES_OCCLUDED].materials[i], true, entity->health());
                        hooks.modelRender.callOriginal<void, void*, void*, const ModelRenderInfo&, matrix3x4*>(21, ctx, state, info, customBoneToWorld);
                        if (!config.chams[ENEMIES_VISIBLE].materials[i].enabled)
                            interfaces.modelRender->forceMaterialOverride(nullptr);
                        applied = true;
                    }
                    if (config.chams[ENEMIES_VISIBLE].materials[i].enabled) {
                        if (applied)
                            hooks.modelRender.callOriginal<void, void*, void*, const ModelRenderInfo&, matrix3x4*>(21, ctx, state, info, customBoneToWorld);
                        applyChams(config.chams[ENEMIES_VISIBLE].materials[i], false, entity->health());
                        hooks.modelRender.callOriginal<void, void*, void*, const ModelRenderInfo&, matrix3x4*>(21, ctx, state, info, customBoneToWorld);
                        needRedraw = false;
                        applied = true;
                    }
                }

                if (config.chams[BACKTRACK].materials[i].enabled && config.backtrack.enabled) {
                    auto record = &Backtrack::records[info.entityIndex];
                    if (record && record->size() && Backtrack::valid(record->front().simulationTime)) {
                        if (applied)
                            hooks.modelRender.callOriginal<void, void*, void*, const ModelRenderInfo&, matrix3x4*>(21, ctx, state, info, customBoneToWorld);
                        applyChams(config.chams[BACKTRACK].materials[i], false, entity->health());
						if (config.backtrack.drawAllTicks) {
							for (int x = 0; x < record->size(); x++) {
								hooks.modelRender.callOriginal<void, void*, void*, const ModelRenderInfo&, matrix3x4*>(21, ctx, state, info, record->at(x).matrix);
							}
						}
						else
							hooks.modelRender.callOriginal<void, void*, void*, const ModelRenderInfo&, matrix3x4*>(21, ctx, state, info, record->back().matrix);
                        interfaces.modelRender->forceMaterialOverride(nullptr);
                        applied = true;
                    }
                }
            } else {
                if (config.chams[ALLIES_ALL].materials[i].enabled) {
                    if (applied)
                        hooks.modelRender.callOriginal<void, void*, void*, const ModelRenderInfo&, matrix3x4*>(21, ctx, state, info, customBoneToWorld);
                    applyChams(config.chams[ALLIES_ALL].materials[i], true, entity->health());
                    hooks.modelRender.callOriginal<void, void*, void*, const ModelRenderInfo&, matrix3x4*>(21, ctx, state, info, customBoneToWorld);
                    applyChams(config.chams[ALLIES_ALL].materials[i], false, entity->health());
                    applied = true;
                } else {
                    if (config.chams[ALLIES_OCCLUDED].materials[i].enabled) {
                        if (applied)
                            hooks.modelRender.callOriginal<void, void*, void*, const ModelRenderInfo&, matrix3x4*>(21, ctx, state, info, customBoneToWorld);
                        applyChams(config.chams[ALLIES_OCCLUDED].materials[i], true, entity->health());
                        hooks.modelRender.callOriginal<void, void*, void*, const ModelRenderInfo&, matrix3x4*>(21, ctx, state, info, customBoneToWorld);
                        if (!config.chams[ALLIES_VISIBLE].materials[i].enabled)
                            interfaces.modelRender->forceMaterialOverride(nullptr);
                        applied = true;
                    }
                    if (config.chams[ALLIES_VISIBLE].materials[i].enabled) {
                        if (applied)
                            hooks.modelRender.callOriginal<void, void*, void*, const ModelRenderInfo&, matrix3x4*>(21, ctx, state, info, customBoneToWorld);
                        applyChams(config.chams[ALLIES_VISIBLE].materials[i], false, entity->health());
                        applied = true;
                    }
                }
            }
        }
    }
    return needRedraw;
}

void Chams::renderWeapons(void* ctx, void* state, const ModelRenderInfo& info, matrix3x4* customBoneToWorld) const noexcept
{
    const auto localPlayer{ interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer()) };
    if (!localPlayer->isScoped()) {
        auto applied{ false };
        for (size_t i = 0; i < config.chams[WEAPONS].materials.size(); i++) {
            if (config.chams[WEAPONS].materials[i].enabled) {
                if (applied)
                    hooks.modelRender.callOriginal<void, void*, void*, const ModelRenderInfo&, matrix3x4*>(21, ctx, state, info, customBoneToWorld);
                applyChams(config.chams[WEAPONS].materials[i], false, localPlayer->health());
                applied = true;
            }
        }
    }    
}

void Chams::renderHands(void* ctx, void* state, const ModelRenderInfo& info, matrix3x4* customBoneToWorld) const noexcept
{
    const auto localPlayer{ interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer()) };
    auto applied{ false };
    for (size_t i = 0; i < config.chams[HANDS].materials.size(); i++) {
        if (config.chams[HANDS].materials[i].enabled) {
            if (applied)
                hooks.modelRender.callOriginal<void, void*, void*, const ModelRenderInfo&, matrix3x4*>(21, ctx, state, info, customBoneToWorld);
            applyChams(config.chams[HANDS].materials[i], false, localPlayer->health());
            applied = true;
        }
    }
}

void Chams::renderSleeves(void* ctx, void* state, const ModelRenderInfo& info, matrix3x4* customBoneToWorld) const noexcept
{
    const auto localPlayer{ interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer()) };
    auto applied{ false };
    for (size_t i = 0; i < config.chams[SLEEVES].materials.size(); i++) {
        if (config.chams[SLEEVES].materials[i].enabled) {
            if (applied)
                hooks.modelRender.callOriginal<void, void*, void*, const ModelRenderInfo&, matrix3x4*>(21, ctx, state, info, customBoneToWorld);
            applyChams(config.chams[SLEEVES].materials[i], false, localPlayer->health());
            applied = true;
        }
    }
}
