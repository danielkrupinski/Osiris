#include <array>

#include "../nlohmann/json.hpp"

#include "../Config.h"
#include "Glow.h"
#include "../Interfaces.h"
#include "../Memory.h"
#include "../SDK/Entity.h"
#include "../SDK/ClientClass.h"
#include "../SDK/GlowObjectManager.h"
#include "../SDK/GlobalVars.h"
#include "../SDK/Utils.h"
#include "../imguiCustom.h"

struct GlowItem : ColorA {
    bool enabled = false;
    bool healthBased = false;
    int style = 0;
};
static std::array<GlowItem, 21> glowConfig;

static std::vector<std::pair<int, int>> customGlowEntities;

void Glow::render() noexcept
{
    if (!localPlayer)
        return;

    const auto& glow = glowConfig;

    Glow::clearCustomObjects();

    for (int i = interfaces->engine->getMaxClients() + 1; i <= interfaces->entityList->getHighestEntityIndex(); ++i) {
        const auto entity = interfaces->entityList->getEntity(i);
        if (!entity || entity->isDormant())
            continue;

        switch (entity->getClientClass()->classId) {
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
            if (!memory->glowObjectManager->hasGlowEffect(entity)) {
                if (auto index{ memory->glowObjectManager->registerGlowObject(entity) }; index != -1)
                    customGlowEntities.emplace_back(i, index);
            }
        }
    }

    for (int i = 0; i < memory->glowObjectManager->glowObjectDefinitions.size; i++) {
        GlowObjectDefinition& glowobject = memory->glowObjectManager->glowObjectDefinitions[i];

        auto entity = glowobject.entity;

        if (glowobject.isUnused() || !entity || entity->isDormant())
            continue;

        auto applyGlow = [&glowobject](decltype(glow[0])& glow, int health = 0) noexcept
        {
            if (glow.enabled) {
                glowobject.renderWhenOccluded = true;
                glowobject.glowAlpha = glow.color[3];
                glowobject.glowStyle = glow.style;
                glowobject.glowAlphaMax = 0.6f;
                if (glow.healthBased && health)
                    glowobject.glowColor = { 1.0f - health / 100.0f,  health / 100.0f, 0.0f };
                else if (glow.rainbow) {
                    const auto [r, g, b] { rainbowColor(glow.rainbowSpeed) };
                    glowobject.glowColor = { r, g, b };
                }
                else
                    glowobject.glowColor = { glow.color[0], glow.color[1], glow.color[2] };
            }
        };

        auto applyPlayerGlow = [applyGlow](decltype(glow[0])& glowAll, decltype(glow[0])& glowVisible, decltype(glow[0])& glowOccluded, Entity* entity) noexcept {
            if (glowAll.enabled)
                applyGlow(glowAll, entity->health());
            else if (glowVisible.enabled && entity->visibleTo(localPlayer.get()))
                applyGlow(glowVisible, entity->health());
            else if (glowOccluded.enabled)
                applyGlow(glowOccluded, entity->health());
        };

        switch (entity->getClientClass()->classId) {
        case ClassId::CSPlayer:
            if (!entity->isAlive())
                break;
            if (auto activeWeapon{ entity->getActiveWeapon() }; activeWeapon && activeWeapon->getClientClass()->classId == ClassId::C4 && activeWeapon->c4StartedArming())
                applyPlayerGlow(glow[6], glow[7], glow[8], entity);
            else if (entity->isDefusing())
                applyPlayerGlow(glow[9], glow[10], glow[11], entity);
            else if (entity == localPlayer.get())
                applyGlow(glow[12], entity->health());
            else if (entity->isOtherEnemy(localPlayer.get()))
                applyPlayerGlow(glow[3], glow[4], glow[5], entity);
            else
                applyPlayerGlow(glow[0], glow[1], glow[2], entity);
            break;
        case ClassId::C4: applyGlow(glow[14]); break;
        case ClassId::PlantedC4: applyGlow(glow[15]); break;
        case ClassId::Chicken: applyGlow(glow[16]); break;
        case ClassId::EconEntity: applyGlow(glow[17]); break;

        case ClassId::BaseCSGrenadeProjectile:
        case ClassId::BreachChargeProjectile:
        case ClassId::BumpMineProjectile:
        case ClassId::DecoyProjectile:
        case ClassId::MolotovProjectile:
        case ClassId::SensorGrenadeProjectile:
        case ClassId::SmokeGrenadeProjectile:
        case ClassId::SnowballProjectile:
            applyGlow(glow[18]); break;

        case ClassId::Hostage: applyGlow(glow[19]); break;
        case ClassId::CSRagdoll: applyGlow(glow[20]); break;
        default:
           if (entity->isWeapon()) {
                applyGlow(glow[13]);
                if (!glow[13].enabled) glowobject.renderWhenOccluded = false;
            }
        }
    }
}

void Glow::clearCustomObjects() noexcept
{
    for (const auto& [entityIndex, glowObjectIndex] : customGlowEntities)
        memory->glowObjectManager->unregisterGlowObject(glowObjectIndex);

    customGlowEntities.clear();
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
    static int currentCategory{ 0 };
    ImGui::PushItemWidth(110.0f);
    ImGui::PushID(0);
    ImGui::Combo("", &currentCategory, "Allies\0Enemies\0Planting\0Defusing\0Local player\0Weapons\0C4\0Planted C4\0Chickens\0Defuse kits\0Projectiles\0Hostages\0Ragdolls\0");
    ImGui::PopID();
    static int currentItem{ 0 };
    if (currentCategory <= 3) {
        ImGui::SameLine();
        static int currentType{ 0 };
        ImGui::PushID(1);
        ImGui::Combo("", &currentType, "All\0Visible\0Occluded\0");
        ImGui::PopID();
        currentItem = currentCategory * 3 + currentType;
    } else {
        currentItem = currentCategory + 8;
    }

    ImGui::SameLine();
    ImGui::Checkbox("Enabled", &glowConfig[currentItem].enabled);
    ImGui::Separator();
    ImGui::Columns(2, nullptr, false);
    ImGui::SetColumnOffset(1, 150.0f);
    ImGui::Checkbox("Health based", &glowConfig[currentItem].healthBased);

    ImGuiCustom::colorPopup("Color", glowConfig[currentItem].color, &glowConfig[currentItem].rainbow, &glowConfig[currentItem].rainbowSpeed);

    ImGui::NextColumn();
    ImGui::SetNextItemWidth(100.0f);
    ImGui::Combo("Style", &glowConfig[currentItem].style, "Default\0Rim3d\0Edge\0Edge Pulse\0");

    ImGui::Columns(1);
    if (!contentOnly)
        ImGui::End();
}

static void to_json(json& j, const GlowItem& o, const GlowItem& dummy = {})
{
    to_json(j, static_cast<const ColorA&>(o), dummy);
    WRITE("Enabled", enabled);
    WRITE("Health based", healthBased);
    WRITE("Style", style);
}

json Glow::toJson() noexcept
{
    json j;
    for (const auto& glowItem : glowConfig)
        j.push_back(glowItem);
    return j;
}

static void from_json(const json& j, GlowItem& g)
{
    from_json(j, static_cast<ColorA&>(g));

    read(j, "Enabled", g.enabled);
    read(j, "Health based", g.healthBased);
    read(j, "Style", g.style);
}

void Glow::fromJson(const json& j) noexcept
{
    if (j.type() == value_t::array && j.size() == glowConfig.size()) {
        for (std::size_t i = 0; i < j.size(); ++i) {
            if (!j[i].empty())
                j[i].get_to(glowConfig[i]);
        }
    }
}

void Glow::resetConfig() noexcept
{
    glowConfig = {};
}
