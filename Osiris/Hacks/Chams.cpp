#include <functional>

#include "Chams.h"
#include "../Config.h"
#include "../Hooks.h"
#include "../Interfaces.h"
#include "Backtrack.h"
#include "../SDK/Material.h"
#include "../SDK/MaterialSystem.h"
#include "../SDK/StudioRender.h"
#include "../SDK/KeyValues.h"

Chams::Chams() noexcept
{
    normal = interfaces->materialSystem->createMaterial("normal", KeyValues::fromString("VertexLitGeneric", nullptr));
    flat = interfaces->materialSystem->createMaterial("flat", KeyValues::fromString("UnlitGeneric", nullptr));
    chrome = interfaces->materialSystem->createMaterial("chrome", KeyValues::fromString("VertexLitGeneric", "$envmap env_cubemap"));
    glow = interfaces->materialSystem->createMaterial("glow", KeyValues::fromString("VertexLitGeneric", "$additive 1 $envmap models/effects/cube_white $envmapfresnel 1 $alpha .8"));

    {
        const auto kv = KeyValues::fromString("VertexLitGeneric", "$envmap editor/cube_vertigo $envmapcontrast 1 $basetexture dev/zone_warning proxies { texturescroll { texturescrollvar $basetexturetransform texturescrollrate 0.6 texturescrollangle 90 } }");
        kv->setString("$envmaptint", "[.7 .7 .7]");
        animated = interfaces->materialSystem->createMaterial("animated", kv);
    }

    {
        const auto kv = KeyValues::fromString("VertexLitGeneric", "$baseTexture models/player/ct_fbi/ct_fbi_glass $envmap env_cubemap");
        kv->setString("$envmaptint", "[.4 .6 .7]");
        platinum = interfaces->materialSystem->createMaterial("platinum", kv);
    }

    {
        const auto kv = KeyValues::fromString("VertexLitGeneric", "$baseTexture detail/dt_metal1 $additive 1 $envmap editor/cube_vertigo");
        kv->setString("$color", "[.05 .05 .05]");
        glass = interfaces->materialSystem->createMaterial("glass", kv);
    }

    {
        const auto kv = KeyValues::fromString("VertexLitGeneric", "$baseTexture black $bumpmap effects/flat_normal $translucent 1 $envmap models/effects/crystal_cube_vertigo_hdr $envmapfresnel 0 $phong 1 $phongexponent 16 $phongboost 2");
        kv->setString("$phongtint", "[.2 .35 .6]");
        crystal = interfaces->materialSystem->createMaterial("crystal", kv);
    }

    {
        const auto kv = KeyValues::fromString("VertexLitGeneric", "$baseTexture white $bumpmap effects/flat_normal $envmap editor/cube_vertigo $envmapfresnel .6 $phong 1 $phongboost 2 $phongexponent 8");
        kv->setString("$color2", "[.05 .05 .05]");
        kv->setString("$envmaptint", "[.2 .2 .2]");
        kv->setString("$phongfresnelranges", "[.7 .8 1]");
        kv->setString("$phongtint", "[.8 .9 1]");
        silver = interfaces->materialSystem->createMaterial("silver", kv);
    }

    {
        const auto kv = KeyValues::fromString("VertexLitGeneric", "$baseTexture white $bumpmap effects/flat_normal $envmap editor/cube_vertigo $envmapfresnel .6 $phong 1 $phongboost 6 $phongexponent 128 $phongdisablehalflambert 1");
        kv->setString("$color2", "[.18 .15 .06]");
        kv->setString("$envmaptint", "[.6 .5 .2]");
        kv->setString("$phongfresnelranges", "[.7 .8 1]");
        kv->setString("$phongtint", "[.6 .5 .2]");
        gold = interfaces->materialSystem->createMaterial("gold", kv);
    }

    {
        const auto kv = KeyValues::fromString("VertexLitGeneric", "$baseTexture black $bumpmap models/inventory_items/trophy_majors/matte_metal_normal $additive 1 $envmap editor/cube_vertigo $envmapfresnel 1 $normalmapalphaenvmapmask 1 $phong 1 $phongboost 20 $phongexponent 3000 $phongdisablehalflambert 1");
        kv->setString("$phongfresnelranges", "[.1 .4 1]");
        kv->setString("$phongtint", "[.8 .9 1]");
        plastic = interfaces->materialSystem->createMaterial("plastic", kv);
    }
}

bool Chams::render(void* ctx, void* state, const ModelRenderInfo& info, matrix3x4* customBoneToWorld) const noexcept
{
    const std::string_view modelName = info.model->name;

    if (modelName.starts_with("models/player")) {
        return renderPlayers(ctx, state, info, customBoneToWorld);
    } else if (modelName.starts_with("models/weapons/v_")) {
        // info.model->name + 17 -> small optimization, skip "models/weapons/v_"
        if (std::strstr(info.model->name + 17, "sleeve"))
            renderSleeves(ctx, state, info, customBoneToWorld);
        else if (std::strstr(info.model->name + 17, "arms"))
            renderHands(ctx, state, info, customBoneToWorld);
        else if (!std::strstr(info.model->name + 17, "tablet")
            && !std::strstr(info.model->name + 17, "parachute")
            && !std::strstr(info.model->name + 17, "fists"))
            renderWeapons(ctx, state, info, customBoneToWorld);
    }

    return true;
}

bool Chams::renderPlayers(void* ctx, void* state, const ModelRenderInfo& info, matrix3x4* customBoneToWorld) const noexcept
{
    if (!localPlayer)
        return true;

    const auto entity = interfaces->entityList->getEntity(info.entityIndex);
    if (!entity || entity->isDormant() || !entity->isPlayer())
        return true;

    bool needRedraw = true;

    auto applied{ false };
    for (size_t i = 0; i < config->chams[ALLIES_ALL].materials.size(); ++i) {
        if (const auto activeWeapon = entity->getActiveWeapon(); activeWeapon && activeWeapon->getClientClass()->classId == ClassId::C4 && activeWeapon->c4StartedArming()
            && (config->chams[PLANTING_ALL].materials[i].enabled || config->chams[PLANTING_OCCLUDED].materials[i].enabled || config->chams[PLANTING_VISIBLE].materials[i].enabled)) {
            if (config->chams[PLANTING_ALL].materials[i].enabled) {
                if (applied)
                    hooks->modelRender.callOriginal<void, 21>(ctx, state, std::cref(info), customBoneToWorld);
                applyChams(config->chams[PLANTING_ALL].materials[i], true, entity->health());
                hooks->modelRender.callOriginal<void, 21>(ctx, state, std::cref(info), customBoneToWorld);
                applyChams(config->chams[PLANTING_ALL].materials[i], false, entity->health());
                applied = true;
            } else {
                if (config->chams[PLANTING_OCCLUDED].materials[i].enabled) {
                    if (applied)
                        hooks->modelRender.callOriginal<void, 21>(ctx, state, std::cref(info), customBoneToWorld);
                    applyChams(config->chams[PLANTING_OCCLUDED].materials[i], true, entity->health());
                    hooks->modelRender.callOriginal<void, 21>(ctx, state, std::cref(info), customBoneToWorld);
                    if (!config->chams[PLANTING_VISIBLE].materials[i].enabled)
                        interfaces->studioRender->forcedMaterialOverride(nullptr);
                    applied = true;
                }

                if (config->chams[PLANTING_VISIBLE].materials[i].enabled) {
                    if (applied)
                        hooks->modelRender.callOriginal<void, 21>(ctx, state, std::cref(info), customBoneToWorld);
                    applyChams(config->chams[PLANTING_VISIBLE].materials[i], false, entity->health());
                    applied = true;
                }
            }
        } else if (entity->isDefusing() && (config->chams[DEFUSING_ALL].materials[i].enabled || config->chams[DEFUSING_OCCLUDED].materials[i].enabled || config->chams[DEFUSING_VISIBLE].materials[i].enabled)) {
            if (config->chams[DEFUSING_ALL].materials[i].enabled) {
                if (applied)
                    hooks->modelRender.callOriginal<void, 21>(ctx, state, std::cref(info), customBoneToWorld);
                applyChams(config->chams[DEFUSING_ALL].materials[i], true, entity->health());
                hooks->modelRender.callOriginal<void, 21>(ctx, state, std::cref(info), customBoneToWorld);
                applyChams(config->chams[DEFUSING_ALL].materials[i], false, entity->health());
                applied = true;
            } else {
                if (config->chams[DEFUSING_OCCLUDED].materials[i].enabled) {
                    if (applied)
                        hooks->modelRender.callOriginal<void, 21>(ctx, state, std::cref(info), customBoneToWorld);
                    applyChams(config->chams[DEFUSING_OCCLUDED].materials[i], true, entity->health());
                    hooks->modelRender.callOriginal<void, 21>(ctx, state, std::cref(info), customBoneToWorld);
                    if (!config->chams[DEFUSING_VISIBLE].materials[i].enabled)
                        interfaces->studioRender->forcedMaterialOverride(nullptr);
                    applied = true;
                }
                if (config->chams[DEFUSING_VISIBLE].materials[i].enabled) {
                    if (applied)
                        hooks->modelRender.callOriginal<void, 21>(ctx, state, std::cref(info), customBoneToWorld);
                    applyChams(config->chams[DEFUSING_VISIBLE].materials[i], false, entity->health());
                    applied = true;
                }
            }
        } else if (info.entityIndex == localPlayer->index()) {
            if (config->chams[LOCALPLAYER].materials[i].enabled) {
                if (applied)
                    hooks->modelRender.callOriginal<void, 21>(ctx, state, std::cref(info), customBoneToWorld);
                applyChams(config->chams[LOCALPLAYER].materials[i], false, entity->health());
                applied = true;
            }
        } else if (entity->isEnemy()) {
            if (config->chams[ENEMIES_ALL].materials[i].enabled) {
                if (applied)
                    hooks->modelRender.callOriginal<void, 21>(ctx, state, std::cref(info), customBoneToWorld);
                applyChams(config->chams[ENEMIES_ALL].materials[i], true, entity->health());
                hooks->modelRender.callOriginal<void, 21>(ctx, state, std::cref(info), customBoneToWorld);
                applyChams(config->chams[ENEMIES_ALL].materials[i], false, entity->health());
                hooks->modelRender.callOriginal<void, 21>(ctx, state, std::cref(info), customBoneToWorld);
                needRedraw = false;
                applied = true;
            } else {
                if (config->chams[ENEMIES_OCCLUDED].materials[i].enabled) {
                    if (applied)
                        hooks->modelRender.callOriginal<void, 21>(ctx, state, std::cref(info), customBoneToWorld);
                    applyChams(config->chams[ENEMIES_OCCLUDED].materials[i], true, entity->health());
                    hooks->modelRender.callOriginal<void, 21>(ctx, state, std::cref(info), customBoneToWorld);
                    if (!config->chams[ENEMIES_VISIBLE].materials[i].enabled)
                        interfaces->studioRender->forcedMaterialOverride(nullptr);
                    applied = true;
                }
                if (config->chams[ENEMIES_VISIBLE].materials[i].enabled) {
                    if (applied)
                        hooks->modelRender.callOriginal<void, 21>(ctx, state, std::cref(info), customBoneToWorld);
                    applyChams(config->chams[ENEMIES_VISIBLE].materials[i], false, entity->health());
                    hooks->modelRender.callOriginal<void, 21>(ctx, state, std::cref(info), customBoneToWorld);
                    needRedraw = false;
                    applied = true;
                }
            }

            if (config->chams[BACKTRACK].materials[i].enabled && config->backtrack.enabled) {
                auto record = &Backtrack::records[info.entityIndex];
                if (record && record->size() && Backtrack::valid(record->front().simulationTime)) {
                    if (applied)
                        hooks->modelRender.callOriginal<void, 21>(ctx, state, std::cref(info), customBoneToWorld);
                    applyChams(config->chams[BACKTRACK].materials[i], false, entity->health());
                    hooks->modelRender.callOriginal<void, 21>(ctx, state, std::cref(info), record->back().matrix);
                    interfaces->studioRender->forcedMaterialOverride(nullptr);
                    applied = true;
                }
            }
        } else {
            if (config->chams[ALLIES_ALL].materials[i].enabled) {
                if (applied)
                    hooks->modelRender.callOriginal<void, 21>(ctx, state, std::cref(info), customBoneToWorld);
                applyChams(config->chams[ALLIES_ALL].materials[i], true, entity->health());
                hooks->modelRender.callOriginal<void, 21>(ctx, state, std::cref(info), customBoneToWorld);
                applyChams(config->chams[ALLIES_ALL].materials[i], false, entity->health());
                applied = true;
            } else {
                if (config->chams[ALLIES_OCCLUDED].materials[i].enabled) {
                    if (applied)
                        hooks->modelRender.callOriginal<void, 21>(ctx, state, std::cref(info), customBoneToWorld);
                    applyChams(config->chams[ALLIES_OCCLUDED].materials[i], true, entity->health());
                    hooks->modelRender.callOriginal<void, 21>(ctx, state, std::cref(info), customBoneToWorld);
                    if (!config->chams[ALLIES_VISIBLE].materials[i].enabled)
                        interfaces->studioRender->forcedMaterialOverride(nullptr);
                    applied = true;
                }
                if (config->chams[ALLIES_VISIBLE].materials[i].enabled) {
                    if (applied)
                        hooks->modelRender.callOriginal<void, 21>(ctx, state, std::cref(info), customBoneToWorld);
                    applyChams(config->chams[ALLIES_VISIBLE].materials[i], false, entity->health());
                    applied = true;
                }
            }
        }
    }
    return needRedraw;
}

void Chams::renderWeapons(void* ctx, void* state, const ModelRenderInfo& info, matrix3x4* customBoneToWorld) const noexcept
{
    if (!localPlayer || !localPlayer->isAlive() || localPlayer->isScoped())
        return;

    bool applied = false;
    for (size_t i = 0; i < config->chams[WEAPONS].materials.size(); ++i) {
        if (config->chams[WEAPONS].materials[i].enabled) {
            if (applied)
                hooks->modelRender.callOriginal<void, 21>(ctx, state, std::cref(info), customBoneToWorld);
            applyChams(config->chams[WEAPONS].materials[i], false, localPlayer->health());
            applied = true;
        }
    }
}

void Chams::renderHands(void* ctx, void* state, const ModelRenderInfo& info, matrix3x4* customBoneToWorld) const noexcept
{
    if (!localPlayer || !localPlayer->isAlive())
        return;

    bool applied = false;
    for (size_t i = 0; i < config->chams[HANDS].materials.size(); ++i) {
        if (config->chams[HANDS].materials[i].enabled) {
            if (applied)
                hooks->modelRender.callOriginal<void, 21>(ctx, state, std::cref(info), customBoneToWorld);
            applyChams(config->chams[HANDS].materials[i], false, localPlayer->health());
            applied = true;
        }
    }
}

void Chams::renderSleeves(void* ctx, void* state, const ModelRenderInfo& info, matrix3x4* customBoneToWorld) const noexcept
{
    if (!localPlayer || !localPlayer->isAlive())
        return;

    bool applied = false;
    for (size_t i = 0; i < config->chams[SLEEVES].materials.size(); ++i) {
        if (config->chams[SLEEVES].materials[i].enabled) {
            if (applied)
                hooks->modelRender.callOriginal<void, 21>(ctx, state, std::cref(info), customBoneToWorld);
            applyChams(config->chams[SLEEVES].materials[i], false, localPlayer->health());
            applied = true;
        }
    }
}
