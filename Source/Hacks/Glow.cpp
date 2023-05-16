#include <algorithm>
#include <array>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include <nlohmann/json.hpp>
#include <imgui/imgui.h>

#include "../ConfigStructs.h"
#include "../InputUtil.h"
#include "Glow.h"
#include "../Helpers.h"
#include "../Memory.h"
#include <CSGO/Constants/ClassId.h>
#include <CSGO/ClientClass.h>
#include <CSGO/Engine.h>
#include <CSGO/Entity.h>
#include <CSGO/EntityList.h>
#include <CSGO/GlobalVars.h>
#include <CSGO/GlowObjectManager.h>
#include <CSGO/LocalPlayer.h>
#include <CSGO/Utils.h>
#include <CSGO/UtlVector.h>
#include <CSGO/Vector.h>
#include "../imguiCustom.h"

#include <Interfaces/ClientInterfaces.h>

Glow::Glow(const ClientPatternFinder& clientPatternFinder)
    : glowObjectManager{ clientPatternFinder.glowObjectManager() }
#if IS_WIN32() || IS_WIN64()
    , glowObjectAntiCheatCheck{ retSpoofGadgets->client, clientPatternFinder.glowObjectAntiCheatCheck() }
#endif
{
}

void Glow::render(const EngineInterfaces& engineInterfaces, const ClientInterfaces& clientInterfaces, const Memory& memory) noexcept
{
    if (!localPlayer)
        return;

    auto& glow = glowConfig;

    Glow::clearCustomObjects();

    if (glowToggleKey.isSet()) {
        if (!glowToggleKey.isToggled() && !glowHoldKey.isDown())
            return;
    } else if (glowHoldKey.isSet() && !glowHoldKey.isDown()) {
        return;
    }

    const auto highestEntityIndex = clientInterfaces.getEntityList().getHighestEntityIndex();
    for (int i = engineInterfaces.getEngine().getMaxClients() + 1; i <= highestEntityIndex; ++i) {
        const auto entity = csgo::Entity::from(retSpoofGadgets->client, clientInterfaces.getEntityList().getEntity(i));
        if (entity.getPOD() == nullptr || entity.getNetworkable().isDormant())
            continue;

        switch (entity.getNetworkable().getClientClass()->classId) {
        case ClassId::EconEntity:
        case ClassId::BaseCSGrenadeProjectile:
        case ClassId::BreachChargeProjectile:
        case ClassId::BumpMineProjectile:
        case ClassId::DecoyProjectile:
        case ClassId::MolotovProjectile:
        case ClassId::SensorGrenadeProjectile:
        case ClassId::SmokeGrenadeProjectile:
        case ClassId::SnowballProjectile:
        case ClassId::Hostage:
        case ClassId::CSRagdoll:
            if (!glowObjectManager->hasGlowEffect(entity.getPOD())) {
                if (auto index{ glowObjectManager->registerGlowObject(entity.getPOD() WIN32_ARGS(, glowObjectAntiCheatCheck)) }; index != -1)
                    customGlowEntities.emplace_back(i, index);
            }
            break;
        default:
            break;
        }
    }

    for (int i = 0; i < glowObjectManager->glowObjectDefinitions.size; i++) {
        csgo::GlowObjectDefinition& glowobject = glowObjectManager->glowObjectDefinitions[i];

        const auto entity = csgo::Entity::from(retSpoofGadgets->client, glowobject.entity);

        if (glowobject.isUnused() || entity.getPOD() == nullptr || entity.getNetworkable().isDormant())
            continue;

        auto applyGlow = [&glowobject, &memory, this](const GlowItem& glow, int health = 0) noexcept
        {
            if (glow.enabled) {
                glowobject.renderWhenOccluded = true;
                glowobject.glowAlpha = glow.color[3];
                glowobject.glowStyle = glow.style;
                glowobject.glowAlphaMax = 0.6f;
                if (glow.healthBased && health) {
                    Helpers::healthColor(std::clamp(health / 100.0f, 0.0f, 1.0f), glowobject.glowColor.x, glowobject.glowColor.y, glowobject.glowColor.z);
                } else if (glow.rainbow) {
                    const auto [r, g, b] { rainbowColor(memory.globalVars->realtime, glow.rainbowSpeed) };
                    glowobject.glowColor = { r, g, b };
                } else {
                    glowobject.glowColor = { glow.color[0], glow.color[1], glow.color[2] };
                }
#if IS_WIN32()
                glowObjectAntiCheatCheck(&glowobject.entity);
#endif
            }
        };

        auto applyPlayerGlow = [this, applyGlow, &memory, &engineInterfaces](const std::string& name, const csgo::Entity& entity) noexcept {
            const auto& cfg = playerGlowConfig[name];
            if (cfg.all.enabled) 
                applyGlow(cfg.all, entity.health());
            else if (cfg.visible.enabled && entity.visibleTo(engineInterfaces, memory, localPlayer.get()))
                applyGlow(cfg.visible, entity.health());
            else if (cfg.occluded.enabled && !entity.visibleTo(engineInterfaces, memory, localPlayer.get()))
                applyGlow(cfg.occluded, entity.health());
        };

        switch (entity.getNetworkable().getClientClass()->classId) {
        case ClassId::CSPlayer:
            if (!entity.isAlive())
                break;
            if (const auto activeWeapon = csgo::Entity::from(retSpoofGadgets->client, entity.getActiveWeapon()); activeWeapon.getPOD() != nullptr && activeWeapon.getNetworkable().getClientClass()->classId == ClassId::C4 && activeWeapon.c4StartedArming())
                applyPlayerGlow("Planting", entity);
            else if (entity.isDefusing())
                applyPlayerGlow("Defusing", entity);
            else if (entity.getPOD() == localPlayer.get().getPOD())
                applyGlow(glow["Local Player"], entity.health());
            else if (entity.isOtherEnemy(memory, localPlayer.get()))
                applyPlayerGlow("Enemies", entity);
            else
                applyPlayerGlow("Allies", entity);
            break;
        case ClassId::C4: applyGlow(glow["C4"]); break;
        case ClassId::PlantedC4: applyGlow(glow["Planted C4"]); break;
        case ClassId::Chicken: applyGlow(glow["Chickens"]); break;
        case ClassId::EconEntity: applyGlow(glow["Defuse Kits"]); break;

        case ClassId::BaseCSGrenadeProjectile:
        case ClassId::BreachChargeProjectile:
        case ClassId::BumpMineProjectile:
        case ClassId::DecoyProjectile:
        case ClassId::MolotovProjectile:
        case ClassId::SensorGrenadeProjectile:
        case ClassId::SmokeGrenadeProjectile:
        case ClassId::SnowballProjectile:
            applyGlow(glow["Projectiles"]); break;

        case ClassId::Hostage: applyGlow(glow["Hostages"]); break;
        case ClassId::CSRagdoll: applyGlow(glow["Ragdolls"]); break;
        default:
           if (entity.isWeapon()) {
                applyGlow(glow["Weapons"]);
                if (!glow["Weapons"].enabled) {
                    glowobject.renderWhenOccluded = false;
#if IS_WIN32()
                    glowObjectAntiCheatCheck(&glowobject.entity);
#endif
                }
            }
        }
    }
}

void Glow::clearCustomObjects() noexcept
{
    for (const auto& [entityIndex, glowObjectIndex] : customGlowEntities)
        glowObjectManager->unregisterGlowObject(glowObjectIndex WIN32_ARGS(, glowObjectAntiCheatCheck));

    customGlowEntities.clear();
}

void Glow::updateInput() noexcept
{
    glowToggleKey.handleToggle();
}

static bool glowWindowOpen = false;

void Glow::menuBarItem() noexcept
{
    if (ImGui::MenuItem("Glow")) {
        glowWindowOpen = true;
        ImGui::SetWindowFocus("Glow");
        ImGui::SetWindowPos("Glow", { 100.0f, 100.0f });
    }
}

void Glow::tabItem() noexcept
{
    if (ImGui::BeginTabItem("Glow")) {
        drawGUI(true);
        ImGui::EndTabItem();
    }
}

void Glow::drawGUI(bool contentOnly) noexcept
{
    if (!contentOnly) {
        if (!glowWindowOpen)
            return;
        ImGui::SetNextWindowSize({ 450.0f, 0.0f });
        ImGui::Begin("Glow", &glowWindowOpen, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
    }

    ImGui::hotkey("Toggle Key", glowToggleKey, 80.0f);
    ImGui::hotkey("Hold Key", glowHoldKey, 80.0f);
    ImGui::Separator();

    static int currentCategory{ 0 };
    ImGui::PushItemWidth(110.0f);
    ImGui::PushID(0);
    constexpr std::array categories{ "Allies", "Enemies", "Planting", "Defusing", "Local Player", "Weapons", "C4", "Planted C4", "Chickens", "Defuse Kits", "Projectiles", "Hostages", "Ragdolls" };
    ImGui::Combo("", &currentCategory, categories.data(), categories.size());
    ImGui::PopID();
    GlowItem* currentItem;
    if (currentCategory <= 3) {
        ImGui::SameLine();
        static int currentType{ 0 };
        ImGui::PushID(1);
        ImGui::Combo("", &currentType, "All\0Visible\0Occluded\0");
        ImGui::PopID();
        auto& cfg = playerGlowConfig[categories[currentCategory]];
        switch (currentType) {
        case 0: currentItem = &cfg.all; break;
        case 1: currentItem = &cfg.visible; break;
        case 2: currentItem = &cfg.occluded; break;
        }
    } else {
        currentItem = &glowConfig[categories[currentCategory]];
    }

    ImGui::SameLine();
    ImGui::Checkbox("Enabled", &currentItem->enabled);
    ImGui::Separator();
    ImGui::Columns(2, nullptr, false);
    ImGui::SetColumnOffset(1, 150.0f);
    ImGui::Checkbox("Health based", &currentItem->healthBased);

    ImGuiCustom::colorPicker("Color", *currentItem);

    ImGui::NextColumn();
    ImGui::SetNextItemWidth(100.0f);
    ImGui::Combo("Style", &currentItem->style, "Default\0Rim3d\0Edge\0Edge Pulse\0");

    ImGui::Columns(1);
    if (!contentOnly)
        ImGui::End();
}

static void to_json(json& j, const Glow::GlowItem& o, const Glow::GlowItem& dummy = {})
{
    to_json(j, static_cast<const Color4&>(o), dummy);
    WRITE("Enabled", enabled);
    WRITE("Health based", healthBased);
    WRITE("Style", style);
}

static void to_json(json& j, const Glow::PlayerGlow& o, const Glow::PlayerGlow& dummy = {})
{
    WRITE("All", all);
    WRITE("Visible", visible);
    WRITE("Occluded", occluded);
}

json Glow::toJson() noexcept
{
    json j;
    j["Items"] = glowConfig;
    j["Players"] = playerGlowConfig;
    to_json(j["Toggle Key"], glowToggleKey, {});
    to_json(j["Hold Key"], glowHoldKey, {});
    return j;
}

static void from_json(const json& j, Glow::GlowItem& g)
{
    from_json(j, static_cast<Color4&>(g));

    read(j, "Enabled", g.enabled);
    read(j, "Health based", g.healthBased);
    read(j, "Style", g.style);
}

static void from_json(const json& j, Glow::PlayerGlow& g)
{
    read<value_t::object>(j, "All", g.all);
    read<value_t::object>(j, "Visible", g.visible);
    read<value_t::object>(j, "Occluded", g.occluded);
}

void Glow::fromJson(const json& j) noexcept
{
    read(j, "Items", glowConfig);
    read(j, "Players", playerGlowConfig);
    read(j, "Toggle Key", glowToggleKey);
    read(j, "Hold Key", glowHoldKey);
}

void Glow::resetConfig() noexcept
{
    glowConfig = {};
    playerGlowConfig = {};
    glowToggleKey = {};
    glowHoldKey = {};
}
