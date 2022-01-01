#include <algorithm>
#include <array>
#include <forward_list>
#include <limits>
#include <numbers>
#include <unordered_map>
#include <vector>

#include "StreamProofESP.h"

#include "../imgui/imgui.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "../imgui/imgui_internal.h"

#include "../Config.h"
#include "../GameData.h"
#include "../Helpers.h"
#include "../InputUtil.h"
#include "../SDK/Engine.h"
#include "../SDK/GlobalVars.h"
#include "../Memory.h"

#include "../imguiCustom.h"

static constexpr auto operator-(float sub, const std::array<float, 3>& a) noexcept
{
    return Vector{ sub - a[0], sub - a[1], sub - a[2] };
}

struct BoundingBox {
private:
    bool valid;
public:
    ImVec2 min, max;
    std::array<ImVec2, 8> vertices;

    BoundingBox(const Vector& mins, const Vector& maxs, const std::array<float, 3>& scale, const matrix3x4* matrix = nullptr) noexcept
    {
        min.y = min.x = (std::numeric_limits<float>::max)();
        max.y = max.x = -(std::numeric_limits<float>::max)();

        const auto scaledMins = mins + (maxs - mins) * 2 * (0.25f - scale);
        const auto scaledMaxs = maxs - (maxs - mins) * 2 * (0.25f - scale);

        for (int i = 0; i < 8; ++i) {
            const Vector point{ i & 1 ? scaledMaxs.x : scaledMins.x,
                                i & 2 ? scaledMaxs.y : scaledMins.y,
                                i & 4 ? scaledMaxs.z : scaledMins.z };

            if (!Helpers::worldToScreenPixelAligned(matrix ? point.transform(*matrix) : point, vertices[i])) {
                valid = false;
                return;
            }

            min.x = (std::min)(min.x, vertices[i].x);
            min.y = (std::min)(min.y, vertices[i].y);
            max.x = (std::max)(max.x, vertices[i].x);
            max.y = (std::max)(max.y, vertices[i].y);
        }
        valid = true;
    }

    BoundingBox(const BaseData& data, const std::array<float, 3>& scale) noexcept : BoundingBox{ data.obbMins, data.obbMaxs, scale, &data.coordinateFrame } {}
    BoundingBox(const Vector& center) noexcept : BoundingBox{ center - 2.0f, center + 2.0f, { 0.25f, 0.25f, 0.25f } } {}

    explicit operator bool() const noexcept
    {
        return valid;
    }
};

static ImDrawList* drawList;

static void addLineWithShadow(const ImVec2& p1, const ImVec2& p2, ImU32 col) noexcept
{
    drawList->AddLine(p1 + ImVec2{ 1.0f, 1.0f }, p2 + ImVec2{ 1.0f, 1.0f }, col & IM_COL32_A_MASK);
    drawList->AddLine(p1, p2, col);
}

// convex hull using Graham's scan
static std::pair<std::array<ImVec2, 8>, std::size_t> convexHull(std::array<ImVec2, 8> points) noexcept
{
    std::swap(points[0], *std::ranges::min_element(points, [](const auto& a, const auto& b) { return a.y < b.y || (a.y == b.y && a.x < b.x); }));

    constexpr auto orientation = [](const ImVec2& a, const ImVec2& b, const ImVec2& c) {
        return (b.x - a.x) * (c.y - a.y) - (c.x - a.x) * (b.y - a.y);
    };

    std::sort(points.begin() + 1, points.end(), [&](const auto& a, const auto& b) {
        const auto o = orientation(points[0], a, b);
        return o == 0.0f ? ImLengthSqr(points[0] - a) < ImLengthSqr(points[0] - b) : o < 0.0f;
    });

    std::array<ImVec2, 8> hull;
    std::size_t count = 0;

    for (const auto& p : points) {
        while (count >= 2 && orientation(hull[count - 2], hull[count - 1], p) >= 0.0f)
            --count;
        hull[count++] = p;
    }

    return std::make_pair(hull, count);
}

static void addRectFilled(const ImVec2& p1, const ImVec2& p2, ImU32 col, bool shadow) noexcept
{
    if (shadow)
        drawList->AddRectFilled(p1 + ImVec2{ 1.0f, 1.0f }, p2 + ImVec2{ 1.0f, 1.0f }, col & IM_COL32_A_MASK);
    drawList->AddRectFilled(p1, p2, col);
}

static void renderBox(const BoundingBox& bbox, const Box& config) noexcept
{
    if (!config.enabled)
        return;

    const ImU32 color = Helpers::calculateColor(config.asColor4());
    const ImU32 fillColor = Helpers::calculateColor(config.fill.asColor4());

    switch (config.type) {
    case Box::_2d:
        if (config.fill.enabled)
            drawList->AddRectFilled(bbox.min + ImVec2{ 1.0f, 1.0f }, bbox.max - ImVec2{ 1.0f, 1.0f }, fillColor, config.rounding, ImDrawFlags_RoundCornersAll);
        else
            drawList->AddRect(bbox.min + ImVec2{ 1.0f, 1.0f }, bbox.max + ImVec2{ 1.0f, 1.0f }, color & IM_COL32_A_MASK, config.rounding, ImDrawFlags_RoundCornersAll);
        drawList->AddRect(bbox.min, bbox.max, color, config.rounding, ImDrawFlags_RoundCornersAll);
        break;
    case Box::_2dCorners: {
        if (config.fill.enabled) {
            drawList->AddRectFilled(bbox.min + ImVec2{ 1.0f, 1.0f }, bbox.max - ImVec2{ 1.0f, 1.0f }, fillColor, config.rounding, ImDrawFlags_RoundCornersAll);
        }

        const bool wantsShadow = !config.fill.enabled;

        const auto quarterWidth = IM_FLOOR((bbox.max.x - bbox.min.x) * 0.25f);
        const auto quarterHeight = IM_FLOOR((bbox.max.y - bbox.min.y) * 0.25f);

        addRectFilled(bbox.min, { bbox.min.x + 1.0f, bbox.min.y + quarterHeight }, color, wantsShadow);
        addRectFilled(bbox.min, { bbox.min.x + quarterWidth, bbox.min.y + 1.0f }, color, wantsShadow);

        addRectFilled({ bbox.max.x, bbox.min.y }, { bbox.max.x - quarterWidth, bbox.min.y + 1.0f }, color, wantsShadow);
        addRectFilled({ bbox.max.x - 1.0f, bbox.min.y }, { bbox.max.x, bbox.min.y + quarterHeight }, color, wantsShadow);

        addRectFilled({ bbox.min.x, bbox.max.y }, { bbox.min.x + 1.0f, bbox.max.y - quarterHeight }, color, wantsShadow);
        addRectFilled({ bbox.min.x, bbox.max.y - 1.0f }, { bbox.min.x + quarterWidth, bbox.max.y }, color, wantsShadow);

        addRectFilled(bbox.max, { bbox.max.x - quarterWidth, bbox.max.y - 1.0f }, color, wantsShadow);
        addRectFilled(bbox.max, { bbox.max.x - 1.0f, bbox.max.y - quarterHeight }, color, wantsShadow);
        break;
    }
    case Box::_3d:
        if (config.fill.enabled) {
            auto [hull, count] = convexHull(bbox.vertices);
            std::reverse(hull.begin(), hull.begin() + count); // make them clockwise for antialiasing
            drawList->AddConvexPolyFilled(hull.data(), count, fillColor);
        } else {
            for (int i = 0; i < 8; ++i) {
                for (int j = 1; j <= 4; j <<= 1) {
                    if (!(i & j))
                        drawList->AddLine(bbox.vertices[i] + ImVec2{ 1.0f, 1.0f }, bbox.vertices[i + j] + ImVec2{ 1.0f, 1.0f }, color & IM_COL32_A_MASK);
                }
            }
        }

        for (int i = 0; i < 8; ++i) {
            for (int j = 1; j <= 4; j <<= 1) {
                if (!(i & j))
                    drawList->AddLine(bbox.vertices[i], bbox.vertices[i + j], color);
            }
        }
        break;
    case Box::_3dCorners:
        if (config.fill.enabled) {
            auto [hull, count] = convexHull(bbox.vertices);
            std::reverse(hull.begin(), hull.begin() + count); // make them clockwise for antialiasing
            drawList->AddConvexPolyFilled(hull.data(), count, fillColor);
        } else {
            for (int i = 0; i < 8; ++i) {
                for (int j = 1; j <= 4; j <<= 1) {
                    if (!(i & j)) {
                        drawList->AddLine(bbox.vertices[i] + ImVec2{ 1.0f, 1.0f }, ImVec2{ bbox.vertices[i].x * 0.75f + bbox.vertices[i + j].x * 0.25f, bbox.vertices[i].y * 0.75f + bbox.vertices[i + j].y * 0.25f } + ImVec2{ 1.0f, 1.0f }, color & IM_COL32_A_MASK);
                        drawList->AddLine(ImVec2{ bbox.vertices[i].x * 0.25f + bbox.vertices[i + j].x * 0.75f, bbox.vertices[i].y * 0.25f + bbox.vertices[i + j].y * 0.75f } + ImVec2{ 1.0f, 1.0f }, bbox.vertices[i + j] + ImVec2{ 1.0f, 1.0f }, color & IM_COL32_A_MASK);
                    }
                }
            }
        }

        for (int i = 0; i < 8; ++i) {
            for (int j = 1; j <= 4; j <<= 1) {
                if (!(i & j)) {
                    drawList->AddLine(bbox.vertices[i], { bbox.vertices[i].x * 0.75f + bbox.vertices[i + j].x * 0.25f, bbox.vertices[i].y * 0.75f + bbox.vertices[i + j].y * 0.25f }, color);
                    drawList->AddLine({ bbox.vertices[i].x * 0.25f + bbox.vertices[i + j].x * 0.75f, bbox.vertices[i].y * 0.25f + bbox.vertices[i + j].y * 0.75f }, bbox.vertices[i + j], color);
                }
            }
        }
        break;
    }
}

static ImVec2 renderText(float distance, float cullDistance, const Color4& textCfg, const char* text, const ImVec2& pos, bool centered = true, bool adjustHeight = true) noexcept
{
    if (cullDistance && Helpers::units2meters(distance) > cullDistance)
        return { };

    const auto textSize = ImGui::CalcTextSize(text);

    const auto horizontalOffset = centered ? textSize.x / 2 : 0.0f;
    const auto verticalOffset = adjustHeight ? textSize.y : 0.0f;

    const auto color = Helpers::calculateColor(textCfg);
    drawList->AddText({ pos.x - horizontalOffset + 1.0f, pos.y - verticalOffset + 1.0f }, color & IM_COL32_A_MASK, text);
    drawList->AddText({ pos.x - horizontalOffset, pos.y - verticalOffset }, color, text);

    return textSize;
}

static void drawSnapline(const Snapline& config, const ImVec2& min, const ImVec2& max) noexcept
{
    if (!config.asColorToggle().enabled)
        return;

    const auto& screenSize = ImGui::GetIO().DisplaySize;

    ImVec2 p1, p2;
    p1.x = screenSize.x / 2;
    p2.x = (min.x + max.x) / 2;

    switch (config.type) {
    case Snapline::Bottom:
        p1.y = screenSize.y;
        p2.y = max.y;
        break;
    case Snapline::Top:
        p1.y = 0.0f;
        p2.y = min.y;
        break;
    case Snapline::Crosshair:
        p1.y = screenSize.y / 2;
        p2.y = (min.y + max.y) / 2;
        break;
    default:
        return;
    }

    drawList->AddLine(p1, p2, Helpers::calculateColor(config.asColorToggle().asColor4()), config.thickness);
}

struct FontPush {
    FontPush(const std::string& name, float distance)
    {
        if (const auto it = config->getFonts().find(name); it != config->getFonts().end()) {
            distance *= GameData::local().fov / 90.0f;

            ImGui::PushFont([](const Config::Font& font, float dist) {
                if (dist <= 400.0f)
                    return font.big;
                if (dist <= 1000.0f)
                    return font.medium;
                return font.tiny;
            }(it->second, distance));
        }
        else {
            ImGui::PushFont(nullptr);
        }
    }

    ~FontPush()
    {
        ImGui::PopFont();
    }
};

static void drawHealthBar(const HealthBar& config, const ImVec2& pos, float height, int health) noexcept
{
    if (!config.enabled)
        return;

    constexpr float width = 3.0f;

    drawList->PushClipRect(pos + ImVec2{ 0.0f, (100 - health) / 100.0f * height }, pos + ImVec2{ width + 1.0f, height + 1.0f });

    if (config.type == HealthBar::Gradient) {
        const auto green = Helpers::calculateColor(0, 255, 0, 255);
        const auto yellow = Helpers::calculateColor(255, 255, 0, 255);
        const auto red = Helpers::calculateColor(255, 0, 0, 255);

        ImVec2 min = pos;
        ImVec2 max = min + ImVec2{ width, height / 2.0f };

        drawList->AddRectFilled(min + ImVec2{ 1.0f, 1.0f }, pos + ImVec2{ width + 1.0f, height + 1.0f }, Helpers::calculateColor(0, 0, 0, 255));

        drawList->AddRectFilledMultiColor(ImFloor(min), ImFloor(max), green, green, yellow, yellow);
        min.y += height / 2.0f;
        max.y += height / 2.0f;
        drawList->AddRectFilledMultiColor(ImFloor(min), ImFloor(max), yellow, yellow, red, red);
    } else {
        const auto color = config.type == HealthBar::HealthBased ? Helpers::healthColor(std::clamp(health / 100.0f, 0.0f, 1.0f)) : Helpers::calculateColor(config.asColor4());
        drawList->AddRectFilled(pos + ImVec2{ 1.0f, 1.0f }, pos + ImVec2{ width + 1.0f, height + 1.0f }, color & IM_COL32_A_MASK);
        drawList->AddRectFilled(pos, pos + ImVec2{ width, height }, color);
    }

    drawList->PopClipRect();
}

static void renderPlayerBox(const PlayerData& playerData, const Player& config) noexcept
{
    const BoundingBox bbox{ playerData, config.box.scale };

    if (!bbox)
        return;

    renderBox(bbox, config.box);

    ImVec2 offsetMins{}, offsetMaxs{};

    drawHealthBar(config.healthBar, bbox.min - ImVec2{ 5.0f, 0.0f }, (bbox.max.y - bbox.min.y), playerData.health);

    FontPush font{ config.font.name, playerData.distanceToLocal };

    if (config.name.enabled) {
        const auto nameSize = renderText(playerData.distanceToLocal, config.textCullDistance, config.name.asColor4(), playerData.name.c_str(), { (bbox.min.x + bbox.max.x) / 2, bbox.min.y - 2 });
        offsetMins.y -= nameSize.y + 2;
    }

    if (config.flashDuration.enabled && playerData.flashDuration > 0.0f) {
        const auto radius = (std::max)(5.0f - playerData.distanceToLocal / 600.0f, 1.0f);
        ImVec2 flashDurationPos{ (bbox.min.x + bbox.max.x) / 2, bbox.min.y + offsetMins.y - radius * 1.5f };

        const auto color = Helpers::calculateColor(config.flashDuration.asColor4());
        constexpr float pi = std::numbers::pi_v<float>;
        drawList->PathArcTo(flashDurationPos + ImVec2{ 1.0f, 1.0f }, radius, pi / 2 - (playerData.flashDuration / 255.0f * pi), pi / 2 + (playerData.flashDuration / 255.0f * pi), 40);
        drawList->PathStroke(color & IM_COL32_A_MASK, false, 0.9f + radius * 0.1f);

        drawList->PathArcTo(flashDurationPos, radius, pi / 2 - (playerData.flashDuration / 255.0f * pi), pi / 2 + (playerData.flashDuration / 255.0f * pi), 40);
        drawList->PathStroke(color, false, 0.9f + radius * 0.1f);

        offsetMins.y -= radius * 2.5f;
    }

    if (config.weapon.enabled && !playerData.activeWeapon.empty()) {
        const auto weaponTextSize = renderText(playerData.distanceToLocal, config.textCullDistance, config.weapon.asColor4(), playerData.activeWeapon.c_str(), { (bbox.min.x + bbox.max.x) / 2, bbox.max.y + 1 }, true, false);
        offsetMaxs.y += weaponTextSize.y + 2.0f;
    }

    drawSnapline(config.snapline, bbox.min + offsetMins, bbox.max + offsetMaxs);

}

static void renderWeaponBox(const WeaponData& weaponData, const Weapon& config) noexcept
{
    const BoundingBox bbox{ weaponData, config.box.scale };

    if (!bbox)
        return;

    renderBox(bbox, config.box);
    drawSnapline(config.snapline, bbox.min, bbox.max);

    FontPush font{ config.font.name, weaponData.distanceToLocal };

    if (config.name.enabled && !weaponData.displayName.empty()) {
        renderText(weaponData.distanceToLocal, config.textCullDistance, config.name.asColor4(), weaponData.displayName.c_str(), { (bbox.min.x + bbox.max.x) / 2, bbox.min.y - 2 });
    }

    if (config.ammo.enabled && weaponData.clip != -1) {
        const auto text{ std::to_string(weaponData.clip) + " / " + std::to_string(weaponData.reserveAmmo) };
        renderText(weaponData.distanceToLocal, config.textCullDistance, config.ammo.asColor4(), text.c_str(), { (bbox.min.x + bbox.max.x) / 2, bbox.max.y + 1 }, true, false);
    }
}

static void renderEntityBox(const BaseData& entityData, const char* name, const Shared& config) noexcept
{
    const BoundingBox bbox{ entityData, config.box.scale };

    if (!bbox)
        return;

    renderBox(bbox, config.box);
    drawSnapline(config.snapline, bbox.min, bbox.max);

    FontPush font{ config.font.name, entityData.distanceToLocal };

    if (config.name.enabled)
        renderText(entityData.distanceToLocal, config.textCullDistance, config.name.asColor4(), name, { (bbox.min.x + bbox.max.x) / 2, bbox.min.y - 5 });
}

static void drawProjectileTrajectory(const Trail& config, const std::vector<std::pair<float, Vector>>& trajectory) noexcept
{
    if (!config.asColorToggle().enabled)
        return;

    std::vector<ImVec2> points, shadowPoints;

    const auto color = Helpers::calculateColor(config.asColorToggle().asColor4());

    for (const auto& [time, point] : trajectory) {
        if (ImVec2 pos; time + config.time >= memory->globalVars->realtime && Helpers::worldToScreen(point, pos)) {
            if (config.type == Trail::Line) {
                points.push_back(pos);
                shadowPoints.push_back(pos + ImVec2{ 1.0f, 1.0f });
            } else if (config.type == Trail::Circles) {
                drawList->AddCircle(pos, 3.5f - point.distTo(GameData::local().origin) / 700.0f, color, 12, config.thickness);
            } else if (config.type == Trail::FilledCircles) {
                drawList->AddCircleFilled(pos, 3.5f - point.distTo(GameData::local().origin) / 700.0f, color);
            }
        }
    }

    if (config.type == Trail::Line) {
        drawList->AddPolyline(shadowPoints.data(), shadowPoints.size(), color & IM_COL32_A_MASK, false, config.thickness);
        drawList->AddPolyline(points.data(), points.size(), color, false, config.thickness);
    }
}

static void drawPlayerSkeleton(const ColorToggleThickness& config, const std::vector<std::pair<Vector, Vector>>& bones) noexcept
{
    if (!config.asColorToggle().enabled)
        return;

    const auto color = Helpers::calculateColor(config.asColorToggle().asColor4());

    std::vector<std::pair<ImVec2, ImVec2>> points, shadowPoints;

    for (const auto& [bone, parent] : bones) {
        ImVec2 bonePoint;
        if (!Helpers::worldToScreenPixelAligned(bone, bonePoint))
            continue;

        ImVec2 parentPoint;
        if (!Helpers::worldToScreenPixelAligned(parent, parentPoint))
            continue;

        points.emplace_back(bonePoint, parentPoint);
        shadowPoints.emplace_back(bonePoint + ImVec2{ 1.0f, 1.0f }, parentPoint + ImVec2{ 1.0f, 1.0f });
    }

    for (const auto& [bonePoint, parentPoint] : shadowPoints)
        drawList->AddLine(bonePoint, parentPoint, color & IM_COL32_A_MASK, config.thickness);

    for (const auto& [bonePoint, parentPoint] : points)
        drawList->AddLine(bonePoint, parentPoint, color, config.thickness);
}

static bool renderPlayerEsp(const PlayerData& playerData, const Player& playerConfig) noexcept
{
    if (!playerConfig.enabled)
        return false;

    if (playerConfig.audibleOnly && !playerData.audible && !playerConfig.spottedOnly
        || playerConfig.spottedOnly && !playerData.spotted && !(playerConfig.audibleOnly && playerData.audible)) // if both "Audible Only" and "Spotted Only" are on treat them as audible OR spotted
        return true;

    if (playerData.immune)
        Helpers::setAlphaFactor(0.5f);

    Helpers::setAlphaFactor(Helpers::getAlphaFactor() * playerData.fadingAlpha());

    renderPlayerBox(playerData, playerConfig);
    drawPlayerSkeleton(playerConfig.skeleton, playerData.bones);

    if (const BoundingBox headBbox{ playerData.headMins, playerData.headMaxs, playerConfig.headBox.scale })
        renderBox(headBbox, playerConfig.headBox);

    Helpers::setAlphaFactor(1.0f);

    return true;
}

static void renderWeaponEsp(const WeaponData& weaponData, const Weapon& parentConfig, const Weapon& itemConfig) noexcept
{
    const auto& config = itemConfig.enabled ? itemConfig : (parentConfig.enabled ? parentConfig : ::config->streamProofESP.weapons["All"]);
    if (config.enabled) {
        renderWeaponBox(weaponData, config);
    }
}

static void renderEntityEsp(const BaseData& entityData, const std::unordered_map<std::string, Shared>& map, const char* name) noexcept
{
    if (const auto cfg = map.find(name); cfg != map.cend() && cfg->second.enabled) {
        renderEntityBox(entityData, name, cfg->second);
    } else if (const auto cfg = map.find("All"); cfg != map.cend() && cfg->second.enabled) {
        renderEntityBox(entityData, name, cfg->second);
    }
}

static void renderProjectileEsp(const ProjectileData& projectileData, const Projectile& parentConfig, const Projectile& itemConfig, const char* name) noexcept
{
    const auto& config = itemConfig.enabled ? itemConfig : parentConfig;

    if (config.enabled) {
        if (!projectileData.exploded)
            renderEntityBox(projectileData, name, config);

        if (config.trails.enabled) {
            if (projectileData.thrownByLocalPlayer)
                drawProjectileTrajectory(config.trails.localPlayer, projectileData.trajectory);
            else if (!projectileData.thrownByEnemy)
                drawProjectileTrajectory(config.trails.allies, projectileData.trajectory);
            else
                drawProjectileTrajectory(config.trails.enemies, projectileData.trajectory);
        }
    }
}

void StreamProofESP::render() noexcept
{
    if (config->streamProofESP.toggleKey.isSet()) {
        if (!config->streamProofESP.toggleKey.isToggled() && !config->streamProofESP.holdKey.isDown())
            return;
    } else if (config->streamProofESP.holdKey.isSet() && !config->streamProofESP.holdKey.isDown()) {
        return;
    }

    drawList = ImGui::GetBackgroundDrawList();

    GameData::Lock lock;

    for (const auto& weapon : GameData::weapons())
        renderWeaponEsp(weapon, config->streamProofESP.weapons[weapon.group], config->streamProofESP.weapons[weapon.name]);

    for (const auto& entity : GameData::entities())
        renderEntityEsp(entity, config->streamProofESP.otherEntities, entity.name);

    for (const auto& lootCrate : GameData::lootCrates()) {
        if (lootCrate.name)
            renderEntityEsp(lootCrate, config->streamProofESP.lootCrates, lootCrate.name);
    }

    for (const auto& projectile : GameData::projectiles())
        renderProjectileEsp(projectile, config->streamProofESP.projectiles["All"], config->streamProofESP.projectiles[projectile.name], projectile.name);

    for (const auto& player : GameData::players()) {
        if ((player.dormant && player.fadingAlpha() == 0.0f) || !player.alive || !player.inViewFrustum)
            continue;

        auto& playerConfig = player.enemy ? config->streamProofESP.enemies : config->streamProofESP.allies;

        if (!renderPlayerEsp(player, playerConfig["All"]))
            renderPlayerEsp(player, playerConfig[player.visible ? "Visible" : "Occluded"]);
    }
}

void StreamProofESP::updateInput() noexcept
{
    config->streamProofESP.toggleKey.handleToggle();
}

static bool windowOpen = false;

void StreamProofESP::menuBarItem() noexcept
{
    if (ImGui::MenuItem("ESP")) {
        windowOpen = true;
        ImGui::SetWindowFocus("ESP");
        ImGui::SetWindowPos("ESP", { 100.0f, 100.0f });
    }
}

void StreamProofESP::tabItem() noexcept
{
    if (ImGui::BeginTabItem("ESP")) {
        drawGUI(true);
        ImGui::EndTabItem();
    }
}

void StreamProofESP::drawGUI(bool contentOnly) noexcept
{
    if (!contentOnly) {
        if (!windowOpen)
            return;
        ImGui::SetNextWindowSize({ 0.0f, 0.0f });
        ImGui::Begin("ESP", &windowOpen, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
    }

    ImGui::hotkey("Toggle Key", config->streamProofESP.toggleKey, 80.0f);
    ImGui::hotkey("Hold Key", config->streamProofESP.holdKey, 80.0f);
    ImGui::Separator();

    static std::size_t currentCategory;
    static auto currentItem = "All";

    constexpr auto getConfigShared = [](std::size_t category, const char* item) noexcept -> Shared& {
        switch (category) {
        case 0: default: return config->streamProofESP.enemies[item];
        case 1: return config->streamProofESP.allies[item];
        case 2: return config->streamProofESP.weapons[item];
        case 3: return config->streamProofESP.projectiles[item];
        case 4: return config->streamProofESP.lootCrates[item];
        case 5: return config->streamProofESP.otherEntities[item];
        }
    };

    constexpr auto getConfigPlayer = [](std::size_t category, const char* item) noexcept -> Player& {
        switch (category) {
        case 0: default: return config->streamProofESP.enemies[item];
        case 1: return config->streamProofESP.allies[item];
        }
    };

    if (ImGui::BeginListBox("##list", { 170.0f, 300.0f })) {
        constexpr std::array categories{ "Enemies", "Allies", "Weapons", "Projectiles", "Loot Crates", "Other Entities" };

        for (std::size_t i = 0; i < categories.size(); ++i) {
            if (ImGui::Selectable(categories[i], currentCategory == i && std::string_view{ currentItem } == "All")) {
                currentCategory = i;
                currentItem = "All";
            }

            if (ImGui::BeginDragDropSource()) {
                switch (i) {
                case 0: case 1: ImGui::SetDragDropPayload("Player", &getConfigPlayer(i, "All"), sizeof(Player), ImGuiCond_Once); break;
                case 2: ImGui::SetDragDropPayload("Weapon", &config->streamProofESP.weapons["All"], sizeof(Weapon), ImGuiCond_Once); break;
                case 3: ImGui::SetDragDropPayload("Projectile", &config->streamProofESP.projectiles["All"], sizeof(Projectile), ImGuiCond_Once); break;
                default: ImGui::SetDragDropPayload("Entity", &getConfigShared(i, "All"), sizeof(Shared), ImGuiCond_Once); break;
                }
                ImGui::EndDragDropSource();
            }

            if (ImGui::BeginDragDropTarget()) {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Player")) {
                    const auto& data = *(Player*)payload->Data;

                    switch (i) {
                    case 0: case 1: getConfigPlayer(i, "All") = data; break;
                    case 2: config->streamProofESP.weapons["All"] = data; break;
                    case 3: config->streamProofESP.projectiles["All"] = data; break;
                    default: getConfigShared(i, "All") = data; break;
                    }
                }

                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Weapon")) {
                    const auto& data = *(Weapon*)payload->Data;

                    switch (i) {
                    case 0: case 1: getConfigPlayer(i, "All") = data; break;
                    case 2: config->streamProofESP.weapons["All"] = data; break;
                    case 3: config->streamProofESP.projectiles["All"] = data; break;
                    default: getConfigShared(i, "All") = data; break;
                    }
                }

                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Projectile")) {
                    const auto& data = *(Projectile*)payload->Data;

                    switch (i) {
                    case 0: case 1: getConfigPlayer(i, "All") = data; break;
                    case 2: config->streamProofESP.weapons["All"] = data; break;
                    case 3: config->streamProofESP.projectiles["All"] = data; break;
                    default: getConfigShared(i, "All") = data; break;
                    }
                }

                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Entity")) {
                    const auto& data = *(Shared*)payload->Data;

                    switch (i) {
                    case 0: case 1: getConfigPlayer(i, "All") = data; break;
                    case 2: config->streamProofESP.weapons["All"] = data; break;
                    case 3: config->streamProofESP.projectiles["All"] = data; break;
                    default: getConfigShared(i, "All") = data; break;
                    }
                }
                ImGui::EndDragDropTarget();
            }

            ImGui::PushID(i);
            ImGui::Indent();

            const auto items = [](std::size_t category) noexcept -> std::vector<const char*> {
                switch (category) {
                case 0:
                case 1: return { "Visible", "Occluded" };
                case 2: return { "Pistols", "SMGs", "Rifles", "Sniper Rifles", "Shotguns", "Machineguns", "Grenades", "Melee", "Other" };
                case 3: return { "Flashbang", "HE Grenade", "Breach Charge", "Bump Mine", "Decoy Grenade", "Molotov", "TA Grenade", "Smoke Grenade", "Snowball" };
                case 4: return { "Pistol Case", "Light Case", "Heavy Case", "Explosive Case", "Tools Case", "Cash Dufflebag" };
                case 5: return { "Defuse Kit", "Chicken", "Planted C4", "Hostage", "Sentry", "Cash", "Ammo Box", "Radar Jammer", "Snowball Pile", "Collectable Coin" };
                default: return { };
                }
            }(i);

            const auto categoryEnabled = getConfigShared(i, "All").enabled;

            for (std::size_t j = 0; j < items.size(); ++j) {
                static bool selectedSubItem;
                if (!categoryEnabled || getConfigShared(i, items[j]).enabled) {
                    if (ImGui::Selectable(items[j], currentCategory == i && !selectedSubItem && std::string_view{ currentItem } == items[j])) {
                        currentCategory = i;
                        currentItem = items[j];
                        selectedSubItem = false;
                    }

                    if (ImGui::BeginDragDropSource()) {
                        switch (i) {
                        case 0: case 1: ImGui::SetDragDropPayload("Player", &getConfigPlayer(i, items[j]), sizeof(Player), ImGuiCond_Once); break;
                        case 2: ImGui::SetDragDropPayload("Weapon", &config->streamProofESP.weapons[items[j]], sizeof(Weapon), ImGuiCond_Once); break;
                        case 3: ImGui::SetDragDropPayload("Projectile", &config->streamProofESP.projectiles[items[j]], sizeof(Projectile), ImGuiCond_Once); break;
                        default: ImGui::SetDragDropPayload("Entity", &getConfigShared(i, items[j]), sizeof(Shared), ImGuiCond_Once); break;
                        }
                        ImGui::EndDragDropSource();
                    }

                    if (ImGui::BeginDragDropTarget()) {
                        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Player")) {
                            const auto& data = *(Player*)payload->Data;

                            switch (i) {
                            case 0: case 1: getConfigPlayer(i, items[j]) = data; break;
                            case 2: config->streamProofESP.weapons[items[j]] = data; break;
                            case 3: config->streamProofESP.projectiles[items[j]] = data; break;
                            default: getConfigShared(i, items[j]) = data; break;
                            }
                        }

                        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Weapon")) {
                            const auto& data = *(Weapon*)payload->Data;

                            switch (i) {
                            case 0: case 1: getConfigPlayer(i, items[j]) = data; break;
                            case 2: config->streamProofESP.weapons[items[j]] = data; break;
                            case 3: config->streamProofESP.projectiles[items[j]] = data; break;
                            default: getConfigShared(i, items[j]) = data; break;
                            }
                        }

                        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Projectile")) {
                            const auto& data = *(Projectile*)payload->Data;

                            switch (i) {
                            case 0: case 1: getConfigPlayer(i, items[j]) = data; break;
                            case 2: config->streamProofESP.weapons[items[j]] = data; break;
                            case 3: config->streamProofESP.projectiles[items[j]] = data; break;
                            default: getConfigShared(i, items[j]) = data; break;
                            }
                        }

                        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Entity")) {
                            const auto& data = *(Shared*)payload->Data;

                            switch (i) {
                            case 0: case 1: getConfigPlayer(i, items[j]) = data; break;
                            case 2: config->streamProofESP.weapons[items[j]] = data; break;
                            case 3: config->streamProofESP.projectiles[items[j]] = data; break;
                            default: getConfigShared(i, items[j]) = data; break;
                            }
                        }
                        ImGui::EndDragDropTarget();
                    }
                }

                if (i != 2)
                    continue;

                ImGui::Indent();

                const auto subItems = [](std::size_t item) noexcept -> std::vector<const char*> {
                    switch (item) {
                    case 0: return { "Glock-18", "P2000", "USP-S", "Dual Berettas", "P250", "Tec-9", "Five-SeveN", "CZ75-Auto", "Desert Eagle", "R8 Revolver" };
                    case 1: return { "MAC-10", "MP9", "MP7", "MP5-SD", "UMP-45", "P90", "PP-Bizon" };
                    case 2: return { "Galil AR", "FAMAS", "AK-47", "M4A4", "M4A1-S", "SG 553", "AUG" };
                    case 3: return { "SSG 08", "AWP", "G3SG1", "SCAR-20" };
                    case 4: return { "Nova", "XM1014", "Sawed-Off", "MAG-7" };
                    case 5: return { "M249", "Negev" };
                    case 6: return { "Flashbang", "HE Grenade", "Smoke Grenade", "Molotov", "Decoy Grenade", "Incendiary", "TA Grenade", "Fire Bomb", "Diversion", "Frag Grenade", "Snowball" };
                    case 7: return { "Axe", "Hammer", "Wrench" };
                    case 8: return { "C4", "Healthshot", "Bump Mine", "Zone Repulsor", "Shield" };
                    default: return { };
                    }
                }(j);

                const auto itemEnabled = getConfigShared(i, items[j]).enabled;

                for (const auto subItem : subItems) {
                    auto& subItemConfig = config->streamProofESP.weapons[subItem];
                    if ((categoryEnabled || itemEnabled) && !subItemConfig.enabled)
                        continue;

                    if (ImGui::Selectable(subItem, currentCategory == i && selectedSubItem && std::string_view{ currentItem } == subItem)) {
                        currentCategory = i;
                        currentItem = subItem;
                        selectedSubItem = true;
                    }

                    if (ImGui::BeginDragDropSource()) {
                        ImGui::SetDragDropPayload("Weapon", &subItemConfig, sizeof(Weapon), ImGuiCond_Once);
                        ImGui::EndDragDropSource();
                    }

                    if (ImGui::BeginDragDropTarget()) {
                        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Player")) {
                            const auto& data = *(Player*)payload->Data;
                            subItemConfig = data;
                        }

                        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Weapon")) {
                            const auto& data = *(Weapon*)payload->Data;
                            subItemConfig = data;
                        }

                        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Projectile")) {
                            const auto& data = *(Projectile*)payload->Data;
                            subItemConfig = data;
                        }

                        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Entity")) {
                            const auto& data = *(Shared*)payload->Data;
                            subItemConfig = data;
                        }
                        ImGui::EndDragDropTarget();
                    }
                }

                ImGui::Unindent();
            }
            ImGui::Unindent();
            ImGui::PopID();
        }
        ImGui::EndListBox();
    }

    ImGui::SameLine();

    if (ImGui::BeginChild("##child", { 400.0f, 0.0f })) {
        auto& sharedConfig = getConfigShared(currentCategory, currentItem);

        ImGui::Checkbox("Enabled", &sharedConfig.enabled);
        ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 260.0f);
        ImGui::SetNextItemWidth(220.0f);
        if (ImGui::BeginCombo("Font", config->getSystemFonts()[sharedConfig.font.index].c_str())) {
            for (size_t i = 0; i < config->getSystemFonts().size(); i++) {
                bool isSelected = config->getSystemFonts()[i] == sharedConfig.font.name;
                if (ImGui::Selectable(config->getSystemFonts()[i].c_str(), isSelected, 0, { 250.0f, 0.0f })) {
                    sharedConfig.font.index = i;
                    sharedConfig.font.name = config->getSystemFonts()[i];
                    config->scheduleFontLoad(sharedConfig.font.name);
                }
                if (isSelected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        ImGui::Separator();

        constexpr auto spacing = 250.0f;
        ImGuiCustom::colorPicker("Snapline", sharedConfig.snapline);
        ImGui::SameLine();
        ImGui::SetNextItemWidth(90.0f);
        ImGui::Combo("##1", &sharedConfig.snapline.type, "Bottom\0Top\0Crosshair\0");
        ImGui::SameLine(spacing);
        ImGuiCustom::colorPicker("Box", sharedConfig.box);
        ImGui::SameLine();

        ImGui::PushID("Box");

        if (ImGui::Button("..."))
            ImGui::OpenPopup("");

        if (ImGui::BeginPopup("")) {
            ImGui::SetNextItemWidth(95.0f);
            ImGui::Combo("Type", &sharedConfig.box.type, "2D\0" "2D corners\0" "3D\0" "3D corners\0");
            ImGui::SetNextItemWidth(275.0f);
            ImGui::SliderFloat3("Scale", sharedConfig.box.scale.data(), 0.0f, 0.50f, "%.2f");
            ImGuiCustom::colorPicker("Fill", sharedConfig.box.fill);
            ImGui::EndPopup();
        }

        ImGui::PopID();

        ImGuiCustom::colorPicker("Name", sharedConfig.name);
        ImGui::SameLine(spacing);

        if (currentCategory < 2) {
            auto& playerConfig = getConfigPlayer(currentCategory, currentItem);

            ImGuiCustom::colorPicker("Weapon", playerConfig.weapon);
            ImGuiCustom::colorPicker("Flash Duration", playerConfig.flashDuration);
            ImGui::SameLine(spacing);
            ImGuiCustom::colorPicker("Skeleton", playerConfig.skeleton);
            ImGui::Checkbox("Audible Only", &playerConfig.audibleOnly);
            ImGui::SameLine(spacing);
            ImGui::Checkbox("Spotted Only", &playerConfig.spottedOnly);

            ImGuiCustom::colorPicker("Head Box", playerConfig.headBox);
            ImGui::SameLine();

            ImGui::PushID("Head Box");

            if (ImGui::Button("..."))
                ImGui::OpenPopup("");

            if (ImGui::BeginPopup("")) {
                ImGui::SetNextItemWidth(95.0f);
                ImGui::Combo("Type", &playerConfig.headBox.type, "2D\0" "2D corners\0" "3D\0" "3D corners\0");
                ImGui::SetNextItemWidth(275.0f);
                ImGui::SliderFloat3("Scale", playerConfig.headBox.scale.data(), 0.0f, 0.50f, "%.2f");
                ImGuiCustom::colorPicker("Fill", playerConfig.headBox.fill);
                ImGui::EndPopup();
            }

            ImGui::PopID();

            ImGui::SameLine(spacing);
            ImGui::Checkbox("Health Bar", &playerConfig.healthBar.enabled);
            ImGui::SameLine();

            ImGui::PushID("Health Bar");

            if (ImGui::Button("..."))
                ImGui::OpenPopup("");

            if (ImGui::BeginPopup("")) {
                ImGui::SetNextItemWidth(95.0f);
                ImGui::Combo("Type", &playerConfig.healthBar.type, "Gradient\0Solid\0Health-based\0");
                if (playerConfig.healthBar.type == HealthBar::Solid) {
                    ImGui::SameLine();
                    ImGuiCustom::colorPicker("", playerConfig.healthBar.asColor4());
                }
                ImGui::EndPopup();
            }

            ImGui::PopID();
        } else if (currentCategory == 2) {
            auto& weaponConfig = config->streamProofESP.weapons[currentItem];
            ImGuiCustom::colorPicker("Ammo", weaponConfig.ammo);
        } else if (currentCategory == 3) {
            auto& trails = config->streamProofESP.projectiles[currentItem].trails;

            ImGui::Checkbox("Trails", &trails.enabled);
            ImGui::SameLine(spacing + 77.0f);
            ImGui::PushID("Trails");

            if (ImGui::Button("..."))
                ImGui::OpenPopup("");

            if (ImGui::BeginPopup("")) {
                constexpr auto trailPicker = [](const char* name, Trail& trail) noexcept {
                    ImGui::PushID(name);
                    ImGuiCustom::colorPicker(name, trail);
                    ImGui::SameLine(150.0f);
                    ImGui::SetNextItemWidth(95.0f);
                    ImGui::Combo("", &trail.type, "Line\0Circles\0Filled Circles\0");
                    ImGui::SameLine();
                    ImGui::SetNextItemWidth(95.0f);
                    ImGui::InputFloat("Time", &trail.time, 0.1f, 0.5f, "%.1fs");
                    trail.time = std::clamp(trail.time, 1.0f, 60.0f);
                    ImGui::PopID();
                };

                trailPicker("Local Player", trails.localPlayer);
                trailPicker("Allies", trails.allies);
                trailPicker("Enemies", trails.enemies);
                ImGui::EndPopup();
            }

            ImGui::PopID();
        }

        ImGui::SetNextItemWidth(95.0f);
        ImGui::InputFloat("Text Cull Distance", &sharedConfig.textCullDistance, 0.4f, 0.8f, "%.1fm");
        sharedConfig.textCullDistance = std::clamp(sharedConfig.textCullDistance, 0.0f, 999.9f);
    }

    ImGui::EndChild();

    if (!contentOnly)
        ImGui::End();
}
