#include <functional>

#include "Chams.h"
#include "../Config.h"
#include "../Hooks.h"
#include "../Interfaces.h"
#include "Backtrack.h"
#include "../SDK/Entity.h"
#include "../SDK/EntityList.h"
#include "../SDK/LocalPlayer.h"
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
    pearlescent = interfaces->materialSystem->createMaterial("pearlescent", KeyValues::fromString("VertexLitGeneric", "$ambientonly 1 $phong 1 $pearlescent 3 $basemapalphaphongmask 1"));
    metallic = interfaces->materialSystem->createMaterial("metallic", KeyValues::fromString("VertexLitGeneric", "$basetexture white $ignorez 0 $envmap env_cubemap $normalmapalphaenvmapmask 1 $envmapcontrast 1 $nofog 1 $model 1 $nocull 0 $selfillum 1 $halfambert 1 $znearer 0 $flat 1"));

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

bool Chams::render(void* ctx, void* state, const ModelRenderInfo& info, matrix3x4* customBoneToWorld) noexcept
{
    const std::string_view modelName = info.model->name;

    appliedChams = false;

    if (modelName.starts_with("models/player")) {
        renderPlayers(ctx, state, info, customBoneToWorld);
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

    return !appliedChams;
}

bool Chams::renderPlayers(void* ctx, void* state, const ModelRenderInfo& info, matrix3x4* customBoneToWorld) noexcept
{
    if (!localPlayer)
        return true;

    const auto entity = interfaces->entityList->getEntity(info.entityIndex);
    if (!entity || entity->isDormant() || !entity->isPlayer())
        return true;

    for (size_t i = 0; i < config->chams[ALLIES_ALL].materials.size(); ++i) {
        if (const auto activeWeapon = entity->getActiveWeapon(); activeWeapon && activeWeapon->getClientClass()->classId == ClassId::C4 && activeWeapon->c4StartedArming()
            && (config->chams[PLANTING_ALL].materials[i].enabled || config->chams[PLANTING_OCCLUDED].materials[i].enabled || config->chams[PLANTING_VISIBLE].materials[i].enabled)) {
            if (config->chams[PLANTING_ALL].materials[i].enabled) {
                applyChams(ctx, state, info, customBoneToWorld, config->chams[PLANTING_ALL].materials[i], true, entity->health());
                applyChams(ctx, state, info, customBoneToWorld, config->chams[PLANTING_ALL].materials[i], false, entity->health());
            } else {
                if (config->chams[PLANTING_OCCLUDED].materials[i].enabled)
                    applyChams(ctx, state, info, customBoneToWorld, config->chams[PLANTING_OCCLUDED].materials[i], true, entity->health());

                if (config->chams[PLANTING_VISIBLE].materials[i].enabled)
                    applyChams(ctx, state, info, customBoneToWorld, config->chams[PLANTING_VISIBLE].materials[i], false, entity->health());
            }
        } else if (entity->isDefusing() && (config->chams[DEFUSING_ALL].materials[i].enabled || config->chams[DEFUSING_OCCLUDED].materials[i].enabled || config->chams[DEFUSING_VISIBLE].materials[i].enabled)) {
            if (config->chams[DEFUSING_ALL].materials[i].enabled) {
                applyChams(ctx, state, info, customBoneToWorld, config->chams[DEFUSING_ALL].materials[i], true, entity->health());
                applyChams(ctx, state, info, customBoneToWorld, config->chams[DEFUSING_ALL].materials[i], false, entity->health());
            } else {
                if (config->chams[DEFUSING_OCCLUDED].materials[i].enabled)
                    applyChams(ctx, state, info, customBoneToWorld, config->chams[DEFUSING_OCCLUDED].materials[i], true, entity->health());

                if (config->chams[DEFUSING_VISIBLE].materials[i].enabled)
                    applyChams(ctx, state, info, customBoneToWorld, config->chams[DEFUSING_VISIBLE].materials[i], false, entity->health());
            }
        } else if (info.entityIndex == localPlayer->index()) {
            if (config->chams[LOCALPLAYER].materials[i].enabled)
                applyChams(ctx, state, info, customBoneToWorld, config->chams[LOCALPLAYER].materials[i], false, entity->health());
        } else if (entity->isOtherEnemy(localPlayer.get())) {
            if (config->chams[ENEMIES_ALL].materials[i].enabled) {
                applyChams(ctx, state, info, customBoneToWorld, config->chams[ENEMIES_ALL].materials[i], true, entity->health());
                applyChams(ctx, state, info, customBoneToWorld, config->chams[ENEMIES_ALL].materials[i], false, entity->health());
            } else {
                if (config->chams[ENEMIES_OCCLUDED].materials[i].enabled)
                    applyChams(ctx, state, info, customBoneToWorld, config->chams[ENEMIES_OCCLUDED].materials[i], true, entity->health());

                if (config->chams[ENEMIES_VISIBLE].materials[i].enabled)
                    applyChams(ctx, state, info, customBoneToWorld, config->chams[ENEMIES_VISIBLE].materials[i], false, entity->health());
            }

            if (config->chams[BACKTRACK].materials[i].enabled && config->backtrack.enabled) {
                auto record = &Backtrack::records[info.entityIndex];
                if (record && record->size() && Backtrack::valid(record->front().simulationTime)) {
                    if (!appliedChams)
                        hooks->modelRender.callOriginal<void, 21>(ctx, state, std::cref(info), customBoneToWorld);
                    applyChams(ctx, state, info, record->back().matrix, config->chams[BACKTRACK].materials[i], false, entity->health());
                    interfaces->studioRender->forcedMaterialOverride(nullptr);
                }
            }
        } else {
            if (config->chams[ALLIES_ALL].materials[i].enabled) {
                applyChams(ctx, state, info, customBoneToWorld, config->chams[ALLIES_ALL].materials[i], true, entity->health());
                applyChams(ctx, state, info, customBoneToWorld, config->chams[ALLIES_ALL].materials[i], false, entity->health());
            } else {
                if (config->chams[ALLIES_OCCLUDED].materials[i].enabled)
                    applyChams(ctx, state, info, customBoneToWorld, config->chams[ALLIES_OCCLUDED].materials[i], true, entity->health());

                if (config->chams[ALLIES_VISIBLE].materials[i].enabled)
                    applyChams(ctx, state, info, customBoneToWorld, config->chams[ALLIES_VISIBLE].materials[i], false, entity->health());
            }
        }
    }
    return true;
}

void Chams::renderWeapons(void* ctx, void* state, const ModelRenderInfo& info, matrix3x4* customBoneToWorld) noexcept
{
    if (!localPlayer || !localPlayer->isAlive() || localPlayer->isScoped())
        return;

    for (size_t i = 0; i < config->chams[WEAPONS].materials.size(); ++i) {
        if (config->chams[WEAPONS].materials[i].enabled)
            applyChams(ctx, state, info, customBoneToWorld, config->chams[WEAPONS].materials[i], false, localPlayer->health());
    }
}

void Chams::renderHands(void* ctx, void* state, const ModelRenderInfo& info, matrix3x4* customBoneToWorld) noexcept
{
    if (!localPlayer || !localPlayer->isAlive())
        return;

    bool applied = false;
    for (size_t i = 0; i < config->chams[HANDS].materials.size(); ++i) {
        if (config->chams[HANDS].materials[i].enabled)
            applyChams(ctx, state, info, customBoneToWorld, config->chams[HANDS].materials[i], false, localPlayer->health());
    }
}

void Chams::renderSleeves(void* ctx, void* state, const ModelRenderInfo& info, matrix3x4* customBoneToWorld) noexcept
{
    if (!localPlayer || !localPlayer->isAlive())
        return;

    bool applied = false;
    for (size_t i = 0; i < config->chams[SLEEVES].materials.size(); ++i) {
        if (config->chams[SLEEVES].materials[i].enabled)
            applyChams(ctx, state, info, customBoneToWorld, config->chams[SLEEVES].materials[i], false, localPlayer->health());
    }
}

void Chams::applyChams(void* ctx, void* state, const ModelRenderInfo& info, matrix3x4* customBoneToWorld, const Config::Chams::Material& chams, bool ignorez, int health) noexcept
{
    const auto material = dispatchMaterial(chams.material);
    if (!material)
        return;

    if (material == glow || material == chrome || material == plastic || material == glass || material == crystal) {
        if (chams.healthBased && health) {
            material->findVar("$envmaptint")->setVectorValue(1.0f - health / 100.0f, health / 100.0f, 0.0f);
        } else if (chams.rainbow) {
            const auto [r, g, b] { rainbowColor(memory->globalVars->realtime, chams.rainbowSpeed) };
            material->findVar("$envmaptint")->setVectorValue(r, g, b);
        } else {
            material->findVar("$envmaptint")->setVectorValue(chams.color[0], chams.color[1], chams.color[2]);
        }
    } else {
        if (chams.healthBased && health) {
            material->colorModulate(1.0f - health / 100.0f, health / 100.0f, 0.0f);
        } else if (chams.rainbow) {
            const auto [r, g, b] { rainbowColor(memory->globalVars->realtime, chams.rainbowSpeed) };
            material->colorModulate(r, g, b);
        } else {
            material->colorModulate(chams.color[0], chams.color[1], chams.color[2]);
        }
    }

    const auto pulse = chams.color[3] * (chams.blinking ? std::sin(memory->globalVars->currenttime * 5) * 0.5f + 0.5f : 1.0f);

    if (material == glow)
        material->findVar("$envmapfresnelminmaxexp")->setVecComponentValue(9.0f * (1.2f - pulse), 2);
    else
        material->alphaModulate(pulse);

    material->setMaterialVarFlag(MaterialVarFlag::IGNOREZ, ignorez);
    material->setMaterialVarFlag(MaterialVarFlag::WIREFRAME, chams.wireframe);
    interfaces->studioRender->forcedMaterialOverride(material);
    hooks->modelRender.callOriginal<void, 21>(ctx, state, std::cref(info), customBoneToWorld);
    appliedChams = true;
}
