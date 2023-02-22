#include <algorithm>
#include <cstring>
#include <deque>
#include <memory>
#include <string>
#include <string_view>
#include <tuple>
#include <unordered_map>

#include "Chams.h"
#include "../Config.h"
#include "../Helpers.h"
#include "../Hooks.h"
#include "../Memory.h"
#include "Backtrack.h"
#include "../InputUtil.h"
#include <CSGO/Constants/ClassId.h>
#include <CSGO/ClientClass.h>
#include <CSGO/Entity.h>
#include <CSGO/EntityList.h>
#include <CSGO/GlobalVars.h>
#include <CSGO/LocalPlayer.h>
#include <CSGO/Material.h>
#include <CSGO/MaterialSystem.h>
#include <CSGO/ModelRender.h>
#include <CSGO/StudioRender.h>
#include <CSGO/KeyValues.h>
#include <CSGO/Utils.h>

#include <Interfaces/ClientInterfaces.h>
#include <Interfaces/OtherInterfaces.h>

#include <Helpers/KeyValuesFactory.h>

static csgo::MaterialPOD* normal;
static csgo::MaterialPOD* flat;
static csgo::MaterialPOD* animated;
static csgo::MaterialPOD* platinum;
static csgo::MaterialPOD* glass;
static csgo::MaterialPOD* crystal;
static csgo::MaterialPOD* chrome;
static csgo::MaterialPOD* silver;
static csgo::MaterialPOD* gold;
static csgo::MaterialPOD* plastic;
static csgo::MaterialPOD* glow;
static csgo::MaterialPOD* pearlescent;
static csgo::MaterialPOD* metallic;

static constexpr auto dispatchMaterial_(int id) noexcept
{
    switch (id) {
    default:
    case 0: return normal;
    case 1: return flat;
    case 2: return animated;
    case 3: return platinum;
    case 4: return glass;
    case 5: return chrome;
    case 6: return crystal;
    case 7: return silver;
    case 8: return gold;
    case 9: return plastic;
    case 10: return glow;
    case 11: return pearlescent;
    case 12: return metallic;
    }
}

static auto dispatchMaterial(int id) noexcept
{
    return csgo::Material::from(retSpoofGadgets->client, dispatchMaterial_(id));
}

void Chams::initializeMaterials(const csgo::MaterialSystem& materialSystem) noexcept
{
    normal = materialSystem.createMaterial("normal", keyValuesFactory("VertexLitGeneric", nullptr).getPOD());
    flat = materialSystem.createMaterial("flat", keyValuesFactory("UnlitGeneric", nullptr).getPOD());

    {
        const auto kv = keyValuesFactory("VertexLitGeneric", nullptr);
        kv.setString("$envmap", "env_cubemap");
        chrome = materialSystem.createMaterial("chrome", kv.getPOD());
    }

    {
        const auto kv = keyValuesFactory("VertexLitGeneric", nullptr);
        kv.setString("$additive", "1");
        kv.setString("$envmap", "models/effects/cube_white");
        kv.setString("$envmapfresnel", "1");
        kv.setString("$alpha", ".8");
        glow = materialSystem.createMaterial("glow", kv.getPOD());
    }

    {
        const auto kv = keyValuesFactory("VertexLitGeneric", nullptr);
        kv.setString("$ambientonly", "1");
        kv.setString("$phong", "1");
        kv.setString("$pearlescent", "3");
        kv.setString("$basemapalphaphongmask", "1");
        pearlescent = materialSystem.createMaterial("pearlescent", kv.getPOD());
    }

    {
        const auto kv = keyValuesFactory("VertexLitGeneric", nullptr);
        kv.setString("$basetexture", "white");
        kv.setString("$ignorez", "0");
        kv.setString("$envmap", "env_cubemap");
        kv.setString("$normalmapalphaenvmapmask", "1");
        kv.setString("$envmapcontrast", "1");
        kv.setString("$nofog", "1");
        kv.setString("$model", "1");
        kv.setString("$nocull", "0");
        kv.setString("$selfillum", "1");
        kv.setString("$halfambert", "1");
        kv.setString("$znearer", "0");
        kv.setString("$flat", "1");
        metallic = materialSystem.createMaterial("metallic", kv.getPOD());
    }

    {
        const auto kv = keyValuesFactory("VertexLitGeneric", nullptr);
        kv.setString("$envmap", "editor/cube_vertigo");
        kv.setString("$envmapcontrast", "1");
        kv.setString("$basetexture", "dev/zone_warning");

        const auto textureScroll = kv.findKey("proxies", true).findKey("texturescroll", true);
        textureScroll.setString("texturescrollvar", "$basetexturetransform");
        textureScroll.setString("texturescrollrate", "0.6");
        textureScroll.setString("texturescrollangle", "90");

        kv.setString("$envmaptint", "[.7 .7 .7]");
        animated = materialSystem.createMaterial("animated", kv.getPOD());
    }

    {
        const auto kv = keyValuesFactory("VertexLitGeneric", nullptr);
        kv.setString("$baseTexture", "models/player/ct_fbi/ct_fbi_glass");
        kv.setString("$envmap", "env_cubemap");
        kv.setString("$envmaptint", "[.4 .6 .7]");
        platinum = materialSystem.createMaterial("platinum", kv.getPOD());
    }

    {
        const auto kv = keyValuesFactory("VertexLitGeneric", nullptr);
        kv.setString("$baseTexture", "detail/dt_metal1");
        kv.setString("$additive", "1");
        kv.setString("$envmap", "editor/cube_vertigo");
        kv.setString("$color", "[.05 .05 .05]");
        glass = materialSystem.createMaterial("glass", kv.getPOD());
    }

    {
        const auto kv = keyValuesFactory("VertexLitGeneric", nullptr);
        kv.setString("$baseTexture", "black");
        kv.setString("$bumpmap", "effects/flat_normal");
        kv.setString("$translucent", "1");
        kv.setString("$envmap", "models/effects/crystal_cube_vertigo_hdr");
        kv.setString("$envmapfresnel", "0");
        kv.setString("$phong", "1");
        kv.setString("$phongexponent", "16");
        kv.setString("$phongboost", "2");
        kv.setString("$phongtint", "[.2 .35 .6]");
        crystal = materialSystem.createMaterial("crystal", kv.getPOD());
    }

    {
        const auto kv = keyValuesFactory("VertexLitGeneric", nullptr);
        kv.setString("$baseTexture", "white");
        kv.setString("$bumpmap", "effects/flat_normal");
        kv.setString("$envmap", "editor/cube_vertigo");
        kv.setString("$envmapfresnel", ".6");
        kv.setString("$phong", "1");
        kv.setString("$phongboost", "2");
        kv.setString("$phongexponent", "8");
        kv.setString("$color2", "[.05 .05 .05]");
        kv.setString("$envmaptint", "[.2 .2 .2]");
        kv.setString("$phongfresnelranges", "[.7 .8 1]");
        kv.setString("$phongtint", "[.8 .9 1]");
        silver = materialSystem.createMaterial("silver", kv.getPOD());
    }

    {
        const auto kv = keyValuesFactory("VertexLitGeneric", nullptr);
        kv.setString("$baseTexture", "white");
        kv.setString("$bumpmap", "effects/flat_normal");
        kv.setString("$envmap", "editor/cube_vertigo");
        kv.setString("$envmapfresnel", ".6");
        kv.setString("$phong", "1");
        kv.setString("$phongboost", "6");
        kv.setString("$phongexponent", "128");
        kv.setString("$phongdisablehalflambert", "1");
        kv.setString("$color2", "[.18 .15 .06]");
        kv.setString("$envmaptint", "[.6 .5 .2]");
        kv.setString("$phongfresnelranges", "[.7 .8 1]");
        kv.setString("$phongtint", "[.6 .5 .2]");
        gold = materialSystem.createMaterial("gold", kv.getPOD());
    }

    {
        const auto kv = keyValuesFactory("VertexLitGeneric", nullptr);
        kv.setString("$baseTexture", "black");
        kv.setString("$bumpmap", "models/inventory_items/trophy_majors/matte_metal_normal");
        kv.setString("$additive", "1");
        kv.setString("$envmap", "editor/cube_vertigo");
        kv.setString("$envmapfresnel", "1");
        kv.setString("$normalmapalphaenvmapmask", "1");
        kv.setString("$phong", "1");
        kv.setString("$phongboost", "20");
        kv.setString("$phongexponent", "3000");
        kv.setString("$phongdisablehalflambert", "1");
        kv.setString("$phongfresnelranges", "[.1 .4 1]");
        kv.setString("$phongtint", "[.8 .9 1]");
        plastic = materialSystem.createMaterial("plastic", kv.getPOD());
    }
}

void Chams::updateInput(Config& config) noexcept
{
    config.chamsToggleKey.handleToggle();
}

bool Chams::render(Backtrack& backtrack, Config& config, void* ctx, void* state, const csgo::ModelRenderInfo& info, csgo::matrix3x4* customBoneToWorld) noexcept
{
    if (config.chamsToggleKey.isSet()) {
        if (!config.chamsToggleKey.isToggled() && !config.chamsHoldKey.isDown())
            return false;
    } else if (config.chamsHoldKey.isSet() && !config.chamsHoldKey.isDown()) {
        return false;
    }

    static bool materialsInitialized = false;
    if (!materialsInitialized) {
        initializeMaterials(interfaces.getMaterialSystem());
        materialsInitialized = true;
    }

    appliedChams = false;
    this->ctx = ctx;
    this->state = state;
    this->info = &info;
    this->customBoneToWorld = customBoneToWorld;

    if (std::string_view{ info.model->name }.starts_with("models/weapons/v_")) {
        // info.model->name + 17 -> small optimization, skip "models/weapons/v_"
        if (std::strstr(info.model->name + 17, "sleeve"))
            renderSleeves(config);
        else if (std::strstr(info.model->name + 17, "arms"))
            renderHands(config);
        else if (!std::strstr(info.model->name + 17, "tablet")
            && !std::strstr(info.model->name + 17, "parachute")
            && !std::strstr(info.model->name + 17, "fists"))
            renderWeapons(config);
    } else {
        const auto entity = csgo::Entity::from(retSpoofGadgets->client, clientInterfaces.getEntityList().getEntity(info.entityIndex));
        if (entity.getPOD() != nullptr && !entity.getNetworkable().isDormant() && entity.isPlayer())
            renderPlayer(backtrack, config, entity);
    }

    return appliedChams;
}

void Chams::renderPlayer(Backtrack& backtrack, Config& config, const csgo::Entity& player) noexcept
{
    if (!localPlayer)
        return;

    const auto health = player.health();

    if (const auto activeWeapon = csgo::Entity::from(retSpoofGadgets->client, player.getActiveWeapon()); activeWeapon.getPOD() != nullptr && activeWeapon.getNetworkable().getClientClass()->classId == ClassId::C4 && activeWeapon.c4StartedArming() && std::ranges::any_of(config.chams["Planting"].materials, [](const Config::Chams::Material& mat) { return mat.enabled; })) {
        applyChams(config.chams["Planting"].materials, health);
    } else if (player.isDefusing() && std::ranges::any_of(config.chams["Defusing"].materials, [](const Config::Chams::Material& mat) { return mat.enabled; })) {
        applyChams(config.chams["Defusing"].materials, health);
    } else if (player.getPOD() == localPlayer.get().getPOD()) {
        applyChams(config.chams["Local player"].materials, health);
    } else if (localPlayer.get().isOtherEnemy(memory, player)) {
        applyChams(config.chams["Enemies"].materials, health);

        const auto records = backtrack.getRecords(player.getNetworkable().index());
        if (records && !records->empty() && backtrack.valid(engineInterfaces.getEngine(), memory, records->front().simulationTime)) {
            if (!appliedChams)
                hooks->modelRenderHooks.getOriginalDrawModelExecute()(engineInterfaces.getPODs().modelRender, ctx, state, info, customBoneToWorld);
            applyChams(config.chams["Backtrack"].materials, health, records->back().matrix);
            interfaces.getStudioRender().forcedMaterialOverride(nullptr);
        }
    } else {
        applyChams(config.chams["Allies"].materials, health);
    }
}

void Chams::renderWeapons(Config& config) noexcept
{
    if (!localPlayer || !localPlayer.get().isAlive() || localPlayer.get().isScoped())
        return;

    applyChams(config.chams["Weapons"].materials, localPlayer.get().health());
}

void Chams::renderHands(Config& config) noexcept
{
    if (!localPlayer || !localPlayer.get().isAlive())
        return;

    applyChams(config.chams["Hands"].materials, localPlayer.get().health());
}

void Chams::renderSleeves(Config& config) noexcept
{
    if (!localPlayer || !localPlayer.get().isAlive())
        return;

    applyChams(config.chams["Sleeves"].materials, localPlayer.get().health());
}

void Chams::applyChams(const std::array<Config::Chams::Material, 7>& chams, int health, csgo::matrix3x4* customMatrix) noexcept
{
    for (const auto& cham : chams) {
        if (!cham.enabled || !cham.ignorez)
            continue;

        const auto material = dispatchMaterial(cham.material);
        if (material.getPOD() == nullptr)
            continue;
        
        float r, g, b;
        if (cham.healthBased && health) {
            Helpers::healthColor(std::clamp(health / 100.0f, 0.0f, 1.0f), r, g, b);
        } else if (cham.rainbow) {
            std::tie(r, g, b) = rainbowColor(memory.globalVars->realtime, cham.rainbowSpeed);
        } else {
            r = cham.color[0];
            g = cham.color[1];
            b = cham.color[2];
        }

        if (material.getPOD() == glow || material.getPOD() == chrome || material.getPOD() == plastic || material.getPOD() == glass || material.getPOD() == crystal)
            csgo::MaterialVar::from(retSpoofGadgets->client, material.findVar("$envmaptint")).setVectorValue(r, g, b);
        else
            material.colorModulate(r, g, b);

        const auto pulse = cham.color[3] * (cham.blinking ? std::sin(memory.globalVars->currenttime * 5) * 0.5f + 0.5f : 1.0f);

        if (material.getPOD() == glow)
            csgo::MaterialVar::from(retSpoofGadgets->client, material.findVar("$envmapfresnelminmaxexp")).setVecComponentValue(9.0f * (1.2f - pulse), 2);
        else
            material.alphaModulate(pulse);

        material.setMaterialVarFlag(MaterialVarFlag::IGNOREZ, true);
        material.setMaterialVarFlag(MaterialVarFlag::WIREFRAME, cham.wireframe);
        interfaces.getStudioRender().forcedMaterialOverride(material.getPOD());
        hooks->modelRenderHooks.getOriginalDrawModelExecute()(engineInterfaces.getPODs().modelRender, ctx, state, info, customMatrix ? customMatrix : customBoneToWorld);
        interfaces.getStudioRender().forcedMaterialOverride(nullptr);
    }

    for (const auto& cham : chams) {
        if (!cham.enabled || cham.ignorez)
            continue;

        const auto material = dispatchMaterial(cham.material);
        if (material.getPOD() == nullptr)
            continue;

        float r, g, b;
        if (cham.healthBased && health) {
            Helpers::healthColor(std::clamp(health / 100.0f, 0.0f, 1.0f), r, g, b);
        } else if (cham.rainbow) {
            std::tie(r, g, b) = rainbowColor(memory.globalVars->realtime, cham.rainbowSpeed);
        } else {
            r = cham.color[0];
            g = cham.color[1];
            b = cham.color[2];
        }

        if (material.getPOD() == glow || material.getPOD() == chrome || material.getPOD() == plastic || material.getPOD() == glass || material.getPOD() == crystal)
            csgo::MaterialVar::from(retSpoofGadgets->client, material.findVar("$envmaptint")).setVectorValue(r, g, b);
        else
            material.colorModulate(r, g, b);

        const auto pulse = cham.color[3] * (cham.blinking ? std::sin(memory.globalVars->currenttime * 5) * 0.5f + 0.5f : 1.0f);

        if (material.getPOD() == glow)
            csgo::MaterialVar::from(retSpoofGadgets->client, material.findVar("$envmapfresnelminmaxexp")).setVecComponentValue(9.0f * (1.2f - pulse), 2);
        else
            material.alphaModulate(pulse);

        if (cham.cover && !appliedChams)
            hooks->modelRenderHooks.getOriginalDrawModelExecute()(engineInterfaces.getPODs().modelRender, ctx, state, info, customMatrix ? customMatrix : customBoneToWorld);

        material.setMaterialVarFlag(MaterialVarFlag::IGNOREZ, false);
        material.setMaterialVarFlag(MaterialVarFlag::WIREFRAME, cham.wireframe);
        interfaces.getStudioRender().forcedMaterialOverride(material.getPOD());
        hooks->modelRenderHooks.getOriginalDrawModelExecute()(engineInterfaces.getPODs().modelRender, ctx, state, info, customMatrix ? customMatrix : customBoneToWorld);
        appliedChams = true;
    }
}
