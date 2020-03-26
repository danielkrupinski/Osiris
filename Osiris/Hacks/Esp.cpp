#define NOMINMAX
#include "Esp.h"
#include "../Config.h"
#include "../Interfaces.h"
#include "../Memory.h"

#include "../SDK/ConVar.h"
#include "../SDK/Entity.h"
#include "../SDK/GlobalVars.h"
#include "../SDK/Localize.h"
#include "../SDK/Surface.h"
#include "../SDK/Vector.h"
#include "../SDK/WeaponData.h"

static constexpr bool worldToScreen(const Vector& in, Vector& out) noexcept
{
    const auto& matrix = interfaces.engine->worldToScreenMatrix();
    float w = matrix._41 * in.x + matrix._42 * in.y + matrix._43 * in.z + matrix._44;

    if (w > 0.001f) {
        const auto [width, height] = interfaces.surface->getScreenSize();
        out.x = width / 2 * (1 + (matrix._11 * in.x + matrix._12 * in.y + matrix._13 * in.z + matrix._14) / w);
        out.y = height / 2 * (1 - (matrix._21 * in.x + matrix._22 * in.y + matrix._23 * in.z + matrix._24) / w);
        out.z = 0.0f;
        return true;
    }
    return false;
}

static void renderSnaplines(Entity* entity, const Config::Esp::Shared& config) noexcept
{
    if (!config.snaplines.enabled)
        return;

    Vector position;
    if (!worldToScreen(entity->getAbsOrigin(), position))
        return;

    if (config.snaplines.rainbow)
        interfaces.surface->setDrawColor(rainbowColor(memory.globalVars->realtime, config.snaplines.rainbowSpeed));
    else
        interfaces.surface->setDrawColor(config.snaplines.color);

    const auto [width, height] = interfaces.surface->getScreenSize();
    interfaces.surface->drawLine(width / 2, height, static_cast<int>(position.x), static_cast<int>(position.y));
}

static void renderEyeTraces(Entity* entity, const Config::Esp::Player& config) noexcept
{
    if (config.eyeTraces.enabled) {
        constexpr float maxRange{ 8192.0f };

        auto eyeAngles = entity->eyeAngles();
        Vector viewAngles{ cos(degreesToRadians(eyeAngles.x)) * cos(degreesToRadians(eyeAngles.y)) * maxRange,
                           cos(degreesToRadians(eyeAngles.x)) * sin(degreesToRadians(eyeAngles.y)) * maxRange,
                          -sin(degreesToRadians(eyeAngles.x)) * maxRange };
        static Trace trace;
        Vector headPosition{ entity->getBonePosition(8) };
        interfaces.engineTrace->traceRay({ headPosition, headPosition + viewAngles }, 0x46004009, { entity }, trace);
        Vector start, end;
        if (worldToScreen(trace.startpos, start) && worldToScreen(trace.endpos, end)) {
            if (config.eyeTraces.rainbow)
                interfaces.surface->setDrawColor(rainbowColor(memory.globalVars->realtime, config.eyeTraces.rainbowSpeed));
            else
                interfaces.surface->setDrawColor(config.eyeTraces.color);

            interfaces.surface->drawLine(start.x, start.y, end.x, end.y);
        }
    }
}

static constexpr void renderPositionedText(unsigned font, const wchar_t* text, std::pair<float, float&> position) noexcept
{
    interfaces.surface->setTextFont(font);
    interfaces.surface->setTextPosition(position.first, position.second);
    position.second += interfaces.surface->getTextSize(font, text).second;
    interfaces.surface->printText(text);
}

struct BoundingBox {
    float x0, y0;
    float x1, y1;
    Vector vertices[8];

    BoundingBox(Entity* entity) noexcept
    {
        const auto [width, height] = interfaces.surface->getScreenSize();

        x0 = static_cast<float>(width * 2);
        y0 = static_cast<float>(height * 2);
        x1 = -x0;
        y1 = -y0;

        const auto& mins = entity->getCollideable()->obbMins();
        const auto& maxs = entity->getCollideable()->obbMaxs();

        for (int i = 0; i < 8; ++i) {
            const Vector point{ i & 1 ? maxs.x : mins.x,
                                i & 2 ? maxs.y : mins.y,
                                i & 4 ? maxs.z : mins.z };

            if (!worldToScreen(point.transform(entity->coordinateFrame()), vertices[i])) {
                valid = false;
                return;
            }
            x0 = std::min(x0, vertices[i].x);
            y0 = std::min(y0, vertices[i].y);
            x1 = std::max(x1, vertices[i].x);
            y1 = std::max(y1, vertices[i].y);
        }
        valid = true;
    }

    operator bool() noexcept
    {
        return valid;
    }
private:
    bool valid;
};

static void renderBox(const BoundingBox& bbox, const Config::Esp::Shared& config) noexcept
{
    if (config.box.enabled) {
        if (config.box.rainbow)
            interfaces.surface->setDrawColor(rainbowColor(memory.globalVars->realtime, config.box.rainbowSpeed));
        else
            interfaces.surface->setDrawColor(config.box.color);
        
        switch (config.boxType) {
        case 0:
            interfaces.surface->drawOutlinedRect(bbox.x0, bbox.y0, bbox.x1, bbox.y1);

            if (config.outline.enabled) {
                if (config.outline.rainbow)
                    interfaces.surface->setDrawColor(rainbowColor(memory.globalVars->realtime, config.outline.rainbowSpeed));
                else
                    interfaces.surface->setDrawColor(config.outline.color);

                interfaces.surface->drawOutlinedRect(bbox.x0 + 1, bbox.y0 + 1, bbox.x1 - 1, bbox.y1 - 1);
                interfaces.surface->drawOutlinedRect(bbox.x0 - 1, bbox.y0 - 1, bbox.x1 + 1, bbox.y1 + 1);
            }
            break;
        case 1:
            interfaces.surface->drawLine(bbox.x0, bbox.y0, bbox.x0, bbox.y0 + fabsf(bbox.y1 - bbox.y0) / 4);
            interfaces.surface->drawLine(bbox.x0, bbox.y0, bbox.x0 + fabsf(bbox.x1 - bbox.x0) / 4, bbox.y0);
            interfaces.surface->drawLine(bbox.x1, bbox.y0, bbox.x1 - fabsf(bbox.x1 - bbox.x0) / 4, bbox.y0);
            interfaces.surface->drawLine(bbox.x1, bbox.y0, bbox.x1, bbox.y0 + fabsf(bbox.y1 - bbox.y0) / 4);
            interfaces.surface->drawLine(bbox.x0, bbox.y1, bbox.x0, bbox.y1 - fabsf(bbox.y1 - bbox.y0) / 4);
            interfaces.surface->drawLine(bbox.x0, bbox.y1, bbox.x0 + fabsf(bbox.x1 - bbox.x0) / 4, bbox.y1);
            interfaces.surface->drawLine(bbox.x1, bbox.y1, bbox.x1 - fabsf(bbox.x1 - bbox.x0) / 4, bbox.y1);
            interfaces.surface->drawLine(bbox.x1, bbox.y1, bbox.x1, bbox.y1 - fabsf(bbox.y1 - bbox.y0) / 4);

            if (config.outline.enabled) {
                if (config.outline.rainbow)
                    interfaces.surface->setDrawColor(rainbowColor(memory.globalVars->realtime, config.outline.rainbowSpeed));
                else
                    interfaces.surface->setDrawColor(config.outline.color);

                interfaces.surface->drawLine(bbox.x0 - 1, bbox.y0 - 1, bbox.x0 - 1, bbox.y0 + fabsf(bbox.y1 - bbox.y0) / 4);
                interfaces.surface->drawLine(bbox.x0 - 1, bbox.y0 - 1, bbox.x0 + fabsf(bbox.x1 - bbox.x0) / 4, bbox.y0 - 1);
                interfaces.surface->drawLine(bbox.x1 + 1, bbox.y0 - 1, bbox.x1 - fabsf(bbox.x1 - bbox.x0) / 4, bbox.y0 - 1);
                interfaces.surface->drawLine(bbox.x1 + 1, bbox.y0 - 1, bbox.x1 + 1, bbox.y0 + fabsf(bbox.y1 - bbox.y0) / 4);
                interfaces.surface->drawLine(bbox.x0 - 1, bbox.y1 + 1, bbox.x0 - 1, bbox.y1 - fabsf(bbox.y1 - bbox.y0) / 4);
                interfaces.surface->drawLine(bbox.x0 - 1, bbox.y1 + 1, bbox.x0 + fabsf(bbox.x1 - bbox.x0) / 4, bbox.y1 + 1);
                interfaces.surface->drawLine(bbox.x1 + 1, bbox.y1 + 1, bbox.x1 - fabsf(bbox.x1 - bbox.x0) / 4, bbox.y1 + 1);
                interfaces.surface->drawLine(bbox.x1 + 1, bbox.y1 + 1, bbox.x1 + 1, bbox.y1 - fabsf(bbox.y1 - bbox.y0) / 4);
            }
            break;
        case 2:
            for (int i = 0; i < 8; i++) {
                for (int j = 1; j <= 4; j <<= 1) {
                    if (!(i & j))
                        interfaces.surface->drawLine(bbox.vertices[i].x, bbox.vertices[i].y, bbox.vertices[i + j].x, bbox.vertices[i + j].y);
                }
            }
            break;
        case 3:
            for (int i = 0; i < 8; i++) {
                for (int j = 1; j <= 4; j <<= 1) {
                    if (!(i & j)) {
                        interfaces.surface->drawLine(bbox.vertices[i].x, bbox.vertices[i].y, bbox.vertices[i].x + (bbox.vertices[i + j].x - bbox.vertices[i].x) * 0.25f, bbox.vertices[i].y + (bbox.vertices[i + j].y - bbox.vertices[i].y) * 0.25f);
                        interfaces.surface->drawLine(bbox.vertices[i].x + (bbox.vertices[i + j].x - bbox.vertices[i].x) * 0.75f, bbox.vertices[i].y + (bbox.vertices[i + j].y - bbox.vertices[i].y) * 0.75f, bbox.vertices[i + j].x, bbox.vertices[i + j].y);
                    }
                }
            }
            break;
        }
    }
}

static void renderPlayerBox(Entity* entity, const Config::Esp::Player& config) noexcept
{
    if (BoundingBox bbox{ entity }) {
        renderBox(bbox, config);

        float drawPositionX = bbox.x0 - 5;

        if (config.healthBar.enabled) {
            static auto gameType{ interfaces.cvar->findVar("game_type") };
            static auto survivalMaxHealth{ interfaces.cvar->findVar("sv_dz_player_max_health") };

            const auto maxHealth{ (std::max)((gameType->getInt() == 6 ? survivalMaxHealth->getInt() : 100), entity->health()) };

            if (config.healthBar.rainbow)
                interfaces.surface->setDrawColor(rainbowColor(memory.globalVars->realtime, config.healthBar.rainbowSpeed));
            else
                interfaces.surface->setDrawColor(config.healthBar.color);

            interfaces.surface->drawFilledRect(drawPositionX - 3, bbox.y0 + abs(bbox.y1 - bbox.y0) * (maxHealth - entity->health()) / static_cast<float>(maxHealth), drawPositionX, bbox.y1);
            
            if (config.outline.enabled) {
                if (config.outline.rainbow)
                    interfaces.surface->setDrawColor(rainbowColor(memory.globalVars->realtime, config.outline.rainbowSpeed));
                else
                    interfaces.surface->setDrawColor(config.outline.color);

                interfaces.surface->drawOutlinedRect(drawPositionX - 4, bbox.y0 - 1, drawPositionX + 1, bbox.y1 + 1);
            }
            drawPositionX -= 7;
        }

        if (config.armorBar.enabled) {
            if (config.armorBar.rainbow)
                interfaces.surface->setDrawColor(rainbowColor(memory.globalVars->realtime, config.armorBar.rainbowSpeed));
            else
                interfaces.surface->setDrawColor(config.armorBar.color);

            interfaces.surface->drawFilledRect(drawPositionX - 3, bbox.y0 + abs(bbox.y1 - bbox.y0) * (100.0f - entity->armor()) / 100.0f, drawPositionX, bbox.y1);

            if (config.outline.enabled) {
                if (config.outline.rainbow)
                    interfaces.surface->setDrawColor(rainbowColor(memory.globalVars->realtime, config.outline.rainbowSpeed));
                else
                    interfaces.surface->setDrawColor(config.outline.color);

                interfaces.surface->drawOutlinedRect(drawPositionX - 4, bbox.y0 - 1, drawPositionX + 1, bbox.y1 + 1);
            }
            drawPositionX -= 7;
        }

        if (config.name.enabled) {
            if (PlayerInfo playerInfo; interfaces.engine->getPlayerInfo(entity->index(), playerInfo)) {
                if (wchar_t name[128]; MultiByteToWideChar(CP_UTF8, 0, playerInfo.name, -1, name, 128)) {
                    const auto [width, height] { interfaces.surface->getTextSize(config.font, name) };
                    interfaces.surface->setTextFont(config.font);
                    if (config.name.rainbow)
                        interfaces.surface->setTextColor(rainbowColor(memory.globalVars->realtime, config.name.rainbowSpeed));
                    else
                        interfaces.surface->setTextColor(config.name.color);

                    interfaces.surface->setTextPosition((bbox.x0 + bbox.x1 - width) / 2, bbox.y0 - 5 - height);
                    interfaces.surface->printText(name);
                }
            }
        }

        if (const auto activeWeapon{ entity->getActiveWeapon() };  config.activeWeapon.enabled && activeWeapon) {
            const auto name{ interfaces.localize->find(activeWeapon->getWeaponData()->name) };
            const auto [width, height] { interfaces.surface->getTextSize(config.font, name) };
            interfaces.surface->setTextFont(config.font);
            if (config.activeWeapon.rainbow)
                interfaces.surface->setTextColor(rainbowColor(memory.globalVars->realtime, config.activeWeapon.rainbowSpeed));
            else
                interfaces.surface->setTextColor(config.activeWeapon.color);

            interfaces.surface->setTextPosition((bbox.x0 + bbox.x1 - width) / 2, bbox.y1 + 5);
            interfaces.surface->printText(name);
        }     

        float drawPositionY = bbox.y0;

        if (config.health.enabled) {
            if (config.health.rainbow)
                interfaces.surface->setTextColor(rainbowColor(memory.globalVars->realtime, config.health.rainbowSpeed));
            else
                interfaces.surface->setTextColor(config.health.color);

            renderPositionedText(config.font, (std::to_wstring(entity->health()) + L" HP").c_str(), { bbox.x1 + 5, drawPositionY });
         }

        if (config.armor.enabled) {
            if (config.armor.rainbow)
                interfaces.surface->setTextColor(rainbowColor(memory.globalVars->realtime, config.armor.rainbowSpeed));
            else
                interfaces.surface->setTextColor(config.armor.color);

            renderPositionedText(config.font, (std::to_wstring(entity->armor()) + L" AR").c_str(), { bbox.x1 + 5, drawPositionY });
        }

        if (config.money.enabled) {
            if (config.money.rainbow)
                interfaces.surface->setTextColor(rainbowColor(memory.globalVars->realtime, config.money.rainbowSpeed));
            else
                interfaces.surface->setTextColor(config.money.color);

            renderPositionedText(config.font, (L'$' + std::to_wstring(entity->account())).c_str(), { bbox.x1 + 5, drawPositionY });
        }

        if (const auto localPlayer{ interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer()) }; config.distance.enabled) {
            if (config.distance.rainbow)
                interfaces.surface->setTextColor(rainbowColor(memory.globalVars->realtime, config.distance.rainbowSpeed));
            else
                interfaces.surface->setTextColor(config.distance.color);

            renderPositionedText(config.font, (std::wostringstream{ } << std::fixed << std::showpoint << std::setprecision(2) << (entity->getAbsOrigin() - localPlayer->getAbsOrigin()).length() * 0.0254f << L'm').str().c_str(), { bbox.x1 + 5, drawPositionY });
        }
    }
}

static void renderWeaponBox(Entity* entity, const Config::Esp::Weapon& config) noexcept
{
    BoundingBox bbox{ entity };

    if (!bbox)
        return;

    renderBox(bbox, config);

    if (config.name.enabled) {
        const auto name{ interfaces.localize->find(entity->getWeaponData()->name) };
        const auto [width, height] { interfaces.surface->getTextSize(config.font, name) };
        interfaces.surface->setTextFont(config.font);
        if (config.name.rainbow)
            interfaces.surface->setTextColor(rainbowColor(memory.globalVars->realtime, config.name.rainbowSpeed));
        else
            interfaces.surface->setTextColor(config.name.color);

        interfaces.surface->setTextPosition((bbox.x0 + bbox.x1 - width) / 2, bbox.y1 + 5);
        interfaces.surface->printText(name);
    }

    float drawPositionY = bbox.y0;

    const auto localPlayer = interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer());
    if (!localPlayer || !config.distance.enabled)
        return;

    if (config.distance.rainbow)
        interfaces.surface->setTextColor(rainbowColor(memory.globalVars->realtime, config.distance.rainbowSpeed));
    else
        interfaces.surface->setTextColor(config.distance.color);

    renderPositionedText(config.font, (std::wostringstream{ } << std::fixed << std::showpoint << std::setprecision(2) << (entity->getAbsOrigin() - localPlayer->getAbsOrigin()).length() * 0.0254f << L'm').str().c_str(), { bbox.x1 + 5, drawPositionY });
}

static void renderEntityBox(Entity* entity, const Config::Esp::Shared& config, const wchar_t* name) noexcept
{
    if (BoundingBox bbox{ entity }) {
        renderBox(bbox, config);

        if (config.name.enabled) {
            const auto [width, height] { interfaces.surface->getTextSize(config.font, name) };
            interfaces.surface->setTextFont(config.font);
            if (config.name.rainbow)
                interfaces.surface->setTextColor(rainbowColor(memory.globalVars->realtime, config.name.rainbowSpeed));
            else
                interfaces.surface->setTextColor(config.name.color);

            interfaces.surface->setTextPosition((bbox.x0 + bbox.x1 - width) / 2, bbox.y1 + 5);
            interfaces.surface->printText(name);
        }

        float drawPositionY = bbox.y0;

        if (const auto localPlayer{ interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer()) }; config.distance.enabled) {
            if (config.distance.rainbow)
                interfaces.surface->setTextColor(rainbowColor(memory.globalVars->realtime, config.distance.rainbowSpeed));
            else
                interfaces.surface->setTextColor(config.distance.color);

            renderPositionedText(config.font, (std::wostringstream{ } << std::fixed << std::showpoint << std::setprecision(2) << (entity->getAbsOrigin() - localPlayer->getAbsOrigin()).length() * 0.0254f << L'm').str().c_str(), { bbox.x1 + 5, drawPositionY });
        }
    }
}

static void renderHeadDot(Entity* entity, const Config::Esp::Player& config) noexcept
{
    if (!config.headDot.enabled)
        return;

    const auto localPlayer = interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer());
    if (!localPlayer)
        return;

    Vector head;
    if (!worldToScreen(entity->getBonePosition(8), head))
        return;

    if (config.headDot.rainbow)
        interfaces.surface->setDrawColor(rainbowColor(memory.globalVars->realtime, config.headDot.rainbowSpeed));
    else
        interfaces.surface->setDrawColor(config.headDot.color);

    interfaces.surface->drawCircle(head.x, head.y, 0, static_cast<int>(100 / std::sqrt((localPlayer->getAbsOrigin() - entity->getAbsOrigin()).length())));
}

enum EspId {
    ALLIES_ALL = 0,
    ALLIES_VISIBLE,
    ALLIES_OCCLUDED,

    ENEMIES_ALL,
    ENEMIES_VISIBLE,
    ENEMIES_OCCLUDED
};

static constexpr bool isInRange(Entity* entity, float maxDistance) noexcept
{
    return maxDistance == 0.0f || (entity->getAbsOrigin() - interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer())->getAbsOrigin()).length() * 0.0254f <= maxDistance;
}

static constexpr bool renderPlayerEsp(Entity* entity, EspId id) noexcept
{
    const auto localPlayer = interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer());
    if ((config.esp.players[id].enabled || 
        config.esp.players[id].deadesp && !localPlayer->isAlive()) &&
        isInRange(entity, config.esp.players[id].maxDistance)) {
        renderSnaplines(entity, config.esp.players[id]);
        renderEyeTraces(entity, config.esp.players[id]);
        renderPlayerBox(entity, config.esp.players[id]);
        renderHeadDot(entity, config.esp.players[id]);
    }
    return config.esp.players[id].enabled;
}

static constexpr void renderWeaponEsp(Entity* entity) noexcept
{
    if (config.esp.weapon.enabled && isInRange(entity, config.esp.weapon.maxDistance)) {
        renderWeaponBox(entity, config.esp.weapon);
        renderSnaplines(entity, config.esp.weapon);
    }
}

static constexpr void renderEntityEsp(Entity* entity, const Config::Esp::Shared& config, const wchar_t* name) noexcept
{
    if (config.enabled && isInRange(entity, config.maxDistance)) {
        renderEntityBox(entity, config, name);
        renderSnaplines(entity, config);
    }
}

void Esp::render() noexcept
{
    if (interfaces.engine->isInGame()) {
        const auto localPlayer = interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer());

        if (!localPlayer)
            return;

        const auto observerTarget = localPlayer->getObserverTarget();

        for (int i = 1; i <= interfaces.engine->getMaxClients(); i++) {
            auto entity = interfaces.entityList->getEntity(i);
            if (!entity || entity == localPlayer || entity == observerTarget
                || entity->isDormant() || !entity->isAlive())
                continue;

            if (!entity->isEnemy()) {
                if (!renderPlayerEsp(entity, ALLIES_ALL)) {
                    if (entity->isVisible())
                        renderPlayerEsp(entity, ALLIES_VISIBLE);
                    else
                        renderPlayerEsp(entity, ALLIES_OCCLUDED);
                }
            } else if (!renderPlayerEsp(entity, ENEMIES_ALL)) {
                if (entity->isVisible())
                    renderPlayerEsp(entity, ENEMIES_VISIBLE);
                else
                    renderPlayerEsp(entity, ENEMIES_OCCLUDED);
            }
        }

        for (int i = interfaces.engine->getMaxClients() + 1; i <= interfaces.entityList->getHighestEntityIndex(); i++) {
            auto entity = interfaces.entityList->getEntity(i);
            if (!entity || entity->isDormant())
                continue;

            if (entity->isWeapon() && entity->ownerEntity() == -1)
                renderWeaponEsp(entity);
            else {
                switch (entity->getClientClass()->classId) {
                case ClassId::Dronegun: {
                    renderEntityEsp(entity, config.esp.dangerZone[0], std::wstring{ interfaces.localize->find("#SFUI_WPNHUD_AutoSentry") }.append(L" (").append(std::to_wstring(entity->sentryHealth())).append(L" HP)").c_str());
                    break;
                }
                case ClassId::Drone: {
                    std::wstring text{ L"Drone" };
                    if (const auto tablet{ interfaces.entityList->getEntityFromHandle(entity->droneTarget()) }) {
                        if (const auto player{ interfaces.entityList->getEntityFromHandle(tablet->ownerEntity()) }) {
                            if (PlayerInfo playerInfo; interfaces.engine->getPlayerInfo(player->index(), playerInfo)) {
                                if (wchar_t name[128]; MultiByteToWideChar(CP_UTF8, 0, playerInfo.name, -1, name, 128)) {
                                    text += L" -> ";
                                    text += name;
                                }
                            }
                        }
                    }
                    renderEntityEsp(entity, config.esp.dangerZone[1], text.c_str());
                    break;
                }
                case ClassId::Cash:
                    renderEntityEsp(entity, config.esp.dangerZone[2], L"Cash");
                    break;
                case ClassId::LootCrate: {
                    const auto modelName{ entity->getModel()->name };
                    if (strstr(modelName, "dufflebag"))
                        renderEntityEsp(entity, config.esp.dangerZone[3], L"Cash Dufflebag");
                    else if (strstr(modelName, "case_pistol"))
                        renderEntityEsp(entity, config.esp.dangerZone[4], L"Pistol Case");
                    else if (strstr(modelName, "case_light"))
                        renderEntityEsp(entity, config.esp.dangerZone[5], L"Light Case");
                    else if (strstr(modelName, "case_heavy"))
                        renderEntityEsp(entity, config.esp.dangerZone[6], L"Heavy Case");
                    else if (strstr(modelName, "case_explosive"))
                        renderEntityEsp(entity, config.esp.dangerZone[7], L"Explosive Case");
                    else if (strstr(modelName, "case_tools"))
                        renderEntityEsp(entity, config.esp.dangerZone[8], L"Tools Case");
                    break;
                }
                case ClassId::WeaponUpgrade: {
                    const auto modelName{ entity->getModel()->name };
                    if (strstr(modelName, "dz_armor_helmet"))
                        renderEntityEsp(entity, config.esp.dangerZone[9], L"Full Armor");
                    else if (strstr(modelName, "dz_armor"))
                        renderEntityEsp(entity, config.esp.dangerZone[10], L"Armor");
                    else if (strstr(modelName, "dz_helmet"))
                        renderEntityEsp(entity, config.esp.dangerZone[11], L"Helmet");
                    else if (strstr(modelName, "parachutepack"))
                        renderEntityEsp(entity, config.esp.dangerZone[12], L"Parachute");
                    else if (strstr(modelName, "briefcase"))
                        renderEntityEsp(entity, config.esp.dangerZone[13], L"Briefcase");
                    else if (strstr(modelName, "upgrade_tablet"))
                        renderEntityEsp(entity, config.esp.dangerZone[14], L"Tablet Upgrade");
                    else if (strstr(modelName, "exojump"))
                        renderEntityEsp(entity, config.esp.dangerZone[15], L"ExoJump");
                    break;
                }
                case ClassId::AmmoBox:
                    renderEntityEsp(entity, config.esp.dangerZone[16], L"Ammobox");
                    break;
                case ClassId::RadarJammer:
                    renderEntityEsp(entity, config.esp.dangerZone[17], interfaces.localize->find("#TabletJammer"));
                    break;
                case ClassId::BaseCSGrenadeProjectile:
                    if (strstr(entity->getModel()->name, "flashbang"))
                        renderEntityEsp(entity, config.esp.projectiles[0], interfaces.localize->find("#SFUI_WPNHUD_Flashbang"));
                    else
                        renderEntityEsp(entity, config.esp.projectiles[1], interfaces.localize->find("#SFUI_WPNHUD_HE_Grenade"));
                    break;
                case ClassId::BreachChargeProjectile:
                    renderEntityEsp(entity, config.esp.projectiles[2], interfaces.localize->find("#SFUI_WPNHUD_BreachCharge"));
                    break;
                case ClassId::BumpMineProjectile:
                    renderEntityEsp(entity, config.esp.projectiles[3], interfaces.localize->find("#SFUI_WPNHUD_BumpMine"));
                    break;
                case ClassId::DecoyProjectile:
                    renderEntityEsp(entity, config.esp.projectiles[4], interfaces.localize->find("#SFUI_WPNHUD_Decoy"));
                    break;
                case ClassId::MolotovProjectile:
                    renderEntityEsp(entity, config.esp.projectiles[5], interfaces.localize->find("#SFUI_WPNHUD_Molotov"));
                    break;
                case ClassId::SensorGrenadeProjectile:
                    renderEntityEsp(entity, config.esp.projectiles[6], interfaces.localize->find("#SFUI_WPNHUD_TAGrenade"));
                    break;
                case ClassId::SmokeGrenadeProjectile:
                    renderEntityEsp(entity, config.esp.projectiles[7], interfaces.localize->find("#SFUI_WPNHUD_SmokeGrenade"));
                    break;
                case ClassId::SnowballProjectile:
                    renderEntityEsp(entity, config.esp.projectiles[8], interfaces.localize->find("#SFUI_WPNHUD_Snowball"));
                    break;
                }
            }   
        }
    }
}
