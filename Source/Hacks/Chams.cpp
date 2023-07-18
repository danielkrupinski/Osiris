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
#include "Chams/ChamsMaterialKeyValuesFactory.h"
#include "Chams/ChamsMaterial.h"

void Chams::updateInput() noexcept
{
    toggleKey.handleToggle();
}

bool Chams::render(Backtrack& backtrack, void* ctx, void* state, const csgo::ModelRenderInfo& info, csgo::matrix3x4* customBoneToWorld) noexcept
{
    if (toggleKey.isSet()) {
        if (!toggleKey.isToggled() && !holdKey.isDown())
            return false;
    } else if (holdKey.isSet() && !holdKey.isDown()) {
        return false;
    }

    appliedChams = false;
    this->ctx = ctx;
    this->state = state;
    this->info = &info;
    this->customBoneToWorld = customBoneToWorld;

    if (std::string_view{ info.model->name }.starts_with("models/weapons/v_")) {
        // info.model->name + 17 -> small optimization, skip "models/weapons/v_"
        if (std::strstr(info.model->name + 17, "sleeve"))
            renderSleeves();
        else if (std::strstr(info.model->name + 17, "arms"))
            renderHands();
        else if (!std::strstr(info.model->name + 17, "tablet")
            && !std::strstr(info.model->name + 17, "parachute")
            && !std::strstr(info.model->name + 17, "fists"))
            renderWeapons();
    } else {
        const auto entity = csgo::Entity::from(retSpoofGadgets->client, clientInterfaces.getEntityList().getEntity(info.entityIndex));
        if (entity.getPOD() != nullptr && !entity.getNetworkable().isDormant() && entity.isPlayer())
            renderPlayer(backtrack, entity);
    }

    return appliedChams;
}

void Chams::renderPlayer(Backtrack& backtrack, const csgo::Entity& player) noexcept
{
    if (!localPlayer)
        return;

    const auto health = player.health();

    if (const auto activeWeapon = csgo::Entity::from(retSpoofGadgets->client, player.getActiveWeapon()); activeWeapon.getPOD() != nullptr && activeWeapon.getNetworkable().getClientClass()->classId == ClassId::C4 && activeWeapon.c4StartedArming() && std::ranges::any_of(chamsMaterials[static_cast<std::size_t>(ChamsCategory::Planting)].materials, [](const ChamsLayer& mat) { return mat.enabled; })) {
        applyChams(chamsMaterials[static_cast<std::size_t>(ChamsCategory::Planting)].materials, health);
    } else if (player.isDefusing() && std::ranges::any_of(chamsMaterials[static_cast<std::size_t>(ChamsCategory::Defusing)].materials, [](const ChamsLayer& mat) { return mat.enabled; })) {
        applyChams(chamsMaterials[static_cast<std::size_t>(ChamsCategory::Defusing)].materials, health);
    } else if (player.getPOD() == localPlayer.get().getPOD()) {
        applyChams(chamsMaterials[static_cast<std::size_t>(ChamsCategory::LocalPlayer)].materials, health);
    } else if (localPlayer.get().isOtherEnemy(memory, player)) {
        applyChams(chamsMaterials[static_cast<std::size_t>(ChamsCategory::Enemies)].materials, health);

        const auto records = backtrack.getRecords(player.getNetworkable().index());
        if (records && !records->empty() && backtrack.valid(engineInterfaces.getEngine(), memory, records->front().simulationTime)) {
            if (!appliedChams)
                modelRenderHooks->getOriginalDrawModelExecute()(std::get<csgo::ModelRenderPOD*>(engineInterfaces.getPODs()), ctx, state, info, customBoneToWorld);
            applyChams(chamsMaterials[static_cast<std::size_t>(ChamsCategory::Backtrack)].materials, health, records->back().matrix);
            interfaces.getStudioRender().forcedMaterialOverride(nullptr);
        }
    } else {
        applyChams(chamsMaterials[static_cast<std::size_t>(ChamsCategory::Allies)].materials, health);
    }
}

void Chams::renderWeapons() noexcept
{
    if (!localPlayer || !localPlayer.get().isAlive() || localPlayer.get().isScoped())
        return;

    applyChams(chamsMaterials[static_cast<std::size_t>(ChamsCategory::Weapons)].materials, localPlayer.get().health());
}

void Chams::renderHands() noexcept
{
    if (!localPlayer || !localPlayer.get().isAlive())
        return;

    applyChams(chamsMaterials[static_cast<std::size_t>(ChamsCategory::Hands)].materials, localPlayer.get().health());
}

void Chams::renderSleeves() noexcept
{
    if (!localPlayer || !localPlayer.get().isAlive())
        return;

    applyChams(chamsMaterials[static_cast<std::size_t>(ChamsCategory::Sleeves)].materials, localPlayer.get().health());
}

void Chams::applyChams(const std::array<ChamsLayer, 7>& chams, int health, csgo::matrix3x4* customMatrix) noexcept
{
    for (const auto& cham : chams) {
        if (!cham.enabled || !cham.ignorez)
            continue;

        const auto material = materials.getMaterial(cham.material);
        if (material.getPOD() == nullptr)
            continue;
        
        float r, g, b;
        if (cham.healthBased && health) {
            Helpers::healthColor(std::clamp(health / 100.0f, 0.0f, 1.0f), r, g, b);
        } else if (cham.color.rainbow) {
            std::tie(r, g, b) = rainbowColor(memory.globalVars->realtime, cham.color.rainbowSpeed);
        } else {
            r = cham.color.color[0];
            g = cham.color.color[1];
            b = cham.color.color[2];
        }

        switch (cham.material) {
        using enum ChamsMaterial;
        case Glow:
        case Chrome:
        case Plastic:
        case Glass:
        case Crystal:
            csgo::MaterialVar::from(retSpoofGadgets->client, material.findVar("$envmaptint")).setVectorValue(r, g, b);
            break;
        default:
            material.colorModulate(r, g, b);
            break;
        }

        const auto pulse = cham.color.color[3] * (cham.blinking ? std::sin(memory.globalVars->currenttime * 5) * 0.5f + 0.5f : 1.0f);

        if (cham.material == ChamsMaterial::Glow)
            csgo::MaterialVar::from(retSpoofGadgets->client, material.findVar("$envmapfresnelminmaxexp")).setVecComponentValue(9.0f * (1.2f - pulse), 2);
        else
            material.alphaModulate(pulse);

        material.setMaterialVarFlag(MaterialVarFlag::IGNOREZ, true);
        material.setMaterialVarFlag(MaterialVarFlag::WIREFRAME, cham.wireframe);
        interfaces.getStudioRender().forcedMaterialOverride(material.getPOD());
        modelRenderHooks->getOriginalDrawModelExecute()(std::get<csgo::ModelRenderPOD*>(engineInterfaces.getPODs()), ctx, state, info, customMatrix ? customMatrix : customBoneToWorld);
        interfaces.getStudioRender().forcedMaterialOverride(nullptr);
    }

    for (const auto& cham : chams) {
        if (!cham.enabled || cham.ignorez)
            continue;

        const auto material = materials.getMaterial(cham.material);
        if (material.getPOD() == nullptr)
            continue;

        float r, g, b;
        if (cham.healthBased && health) {
            Helpers::healthColor(std::clamp(health / 100.0f, 0.0f, 1.0f), r, g, b);
        } else if (cham.color.rainbow) {
            std::tie(r, g, b) = rainbowColor(memory.globalVars->realtime, cham.color.rainbowSpeed);
        } else {
            r = cham.color.color[0];
            g = cham.color.color[1];
            b = cham.color.color[2];
        }

        switch (cham.material) {
            using enum ChamsMaterial;
        case Glow:
        case Chrome:
        case Plastic:
        case Glass:
        case Crystal:
            csgo::MaterialVar::from(retSpoofGadgets->client, material.findVar("$envmaptint")).setVectorValue(r, g, b);
            break;
        default:
            material.colorModulate(r, g, b);
            break;
        }

        const auto pulse = cham.color.color[3] * (cham.blinking ? std::sin(memory.globalVars->currenttime * 5) * 0.5f + 0.5f : 1.0f);

        if (cham.material == ChamsMaterial::Glow)
            csgo::MaterialVar::from(retSpoofGadgets->client, material.findVar("$envmapfresnelminmaxexp")).setVecComponentValue(9.0f * (1.2f - pulse), 2);
        else
            material.alphaModulate(pulse);

        if (cham.cover && !appliedChams)
            modelRenderHooks->getOriginalDrawModelExecute()(std::get<csgo::ModelRenderPOD*>(engineInterfaces.getPODs()), ctx, state, info, customMatrix ? customMatrix : customBoneToWorld);

        material.setMaterialVarFlag(MaterialVarFlag::IGNOREZ, false);
        material.setMaterialVarFlag(MaterialVarFlag::WIREFRAME, cham.wireframe);
        interfaces.getStudioRender().forcedMaterialOverride(material.getPOD());
        modelRenderHooks->getOriginalDrawModelExecute()(std::get<csgo::ModelRenderPOD*>(engineInterfaces.getPODs()), ctx, state, info, customMatrix ? customMatrix : customBoneToWorld);
        appliedChams = true;
    }
}
