#include "Esp.h"
#include "../Config.h"
#include "../Interfaces.h"
#include "../SDK/ConVar.h"
#include "../SDK/Entity.h"
#include "../SDK/Localize.h"
#include "../SDK/Surface.h"
#include "../SDK/Vector.h"
#include "../SDK/WeaponData.h"

static constexpr bool worldToScreen(const Vector& in, Vector& out) noexcept
{
    const auto matrix = interfaces.engine->worldToScreenMatrix();
    float w = matrix[3][0] * in.x + matrix[3][1] * in.y + matrix[3][2] * in.z + matrix[3][3];

    if (w > 0.001f) {
        const auto [width, height] = interfaces.surface->getScreenSize();
        out.x = width / 2 * (1 + (matrix[0][0] * in.x + matrix[0][1] * in.y + matrix[0][2] * in.z + matrix[0][3]) / w);
        out.y = height / 2 * (1 - (matrix[1][0] * in.x + matrix[1][1] * in.y + matrix[1][2] * in.z + matrix[1][3]) / w);
        out.z = 0.0f;
        return true;
    }
    return false;
}

static constexpr void renderSnaplines(Entity* entity, const Config::Esp::Shared& config) noexcept
{
    if (config.snaplines) {
        Vector position{ };
        if (worldToScreen(entity->getAbsOrigin(), position)) {
            const auto [width, height] = interfaces.surface->getScreenSize();
            interfaces.surface->setDrawColor(config.snaplinesColor, 255);
            interfaces.surface->drawLine(width / 2, height, static_cast<int>(position.x), static_cast<int>(position.y));
        }
    }
}

static void renderEyeTraces(Entity* entity, const Config::Esp::Player& config) noexcept
{
    if (config.eyeTraces) {
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
            interfaces.surface->setDrawColor(config.eyeTracesColor, 255);
            interfaces.surface->drawLine(start.x, start.y, end.x, end.y);
        }
    }
}

static constexpr void renderPositionedText(unsigned font, const wchar_t* text, const float color[3], std::pair<float, float&> position) noexcept
{
    interfaces.surface->setTextFont(font);
    interfaces.surface->setTextColor(color, 255);
    interfaces.surface->setTextPosition(position.first, position.second);
    position.second += interfaces.surface->getTextSize(font, text).second;
    interfaces.surface->printText(text);
}

struct BoundingBox {
    float left;
    float right;
    float top;
    float bottom;
    Vector vertices[8];
};

static auto boundingBox(Entity* entity, BoundingBox& out) noexcept
{
    const auto [width, height] { interfaces.surface->getScreenSize() };
    out.left = static_cast<float>(width * 2);
    out.right = -static_cast<float>(width * 2);
    out.top = -static_cast<float>(height * 2);
    out.bottom = static_cast<float>(height * 2);

    const auto min{ entity->getCollideable()->obbMins() };
    const auto max{ entity->getCollideable()->obbMaxs() };

    for (int i = 0; i < 8; i++) {
        const Vector point{ i & 1 ? max.x : min.x,
                            i & 2 ? max.y : min.y,
                            i & 4 ? max.z : min.z };

        if (!worldToScreen(point.transform(entity->coordinateFrame()), out.vertices[i]))
            return false;

        if (out.left > out.vertices[i].x)
            out.left = out.vertices[i].x;

        if (out.right < out.vertices[i].x)
            out.right = out.vertices[i].x;

        if (out.top < out.vertices[i].y)
            out.top = out.vertices[i].y;

        if (out.bottom > out.vertices[i].y)
            out.bottom = out.vertices[i].y;
    }
    return true;
}

static void renderBox(Entity* entity, const BoundingBox& bbox, const Config::Esp::Shared& config) noexcept
{
    if (config.box) {
        interfaces.surface->setDrawColor(config.boxColor, 255);

        switch (config.boxType) {
        case 0:
            interfaces.surface->drawOutlinedRect(bbox.left, bbox.bottom, bbox.right, bbox.top);

            if (config.outline) {
                interfaces.surface->setDrawColor(config.outlineColor, 255);
                interfaces.surface->drawOutlinedRect(bbox.left + 1, bbox.bottom + 1, bbox.right - 1, bbox.top - 1);
                interfaces.surface->drawOutlinedRect(bbox.left - 1, bbox.bottom - 1, bbox.right + 1, bbox.top + 1);
            }
            break;
        case 1:
            interfaces.surface->drawLine(bbox.left, bbox.bottom, bbox.left, bbox.bottom + fabsf(bbox.top - bbox.bottom) / 4);
            interfaces.surface->drawLine(bbox.left, bbox.bottom, bbox.left + fabsf(bbox.right - bbox.left) / 4, bbox.bottom);
            interfaces.surface->drawLine(bbox.right, bbox.bottom, bbox.right - fabsf(bbox.right - bbox.left) / 4, bbox.bottom);
            interfaces.surface->drawLine(bbox.right, bbox.bottom, bbox.right, bbox.bottom + fabsf(bbox.top - bbox.bottom) / 4);
            interfaces.surface->drawLine(bbox.left, bbox.top, bbox.left, bbox.top - fabsf(bbox.top - bbox.bottom) / 4);
            interfaces.surface->drawLine(bbox.left, bbox.top, bbox.left + fabsf(bbox.right - bbox.left) / 4, bbox.top);
            interfaces.surface->drawLine(bbox.right, bbox.top, bbox.right - fabsf(bbox.right - bbox.left) / 4, bbox.top);
            interfaces.surface->drawLine(bbox.right, bbox.top, bbox.right, bbox.top - fabsf(bbox.top - bbox.bottom) / 4);

            if (config.outline) {
                interfaces.surface->setDrawColor(config.outlineColor, 255);
                interfaces.surface->drawLine(bbox.left - 1, bbox.bottom - 1, bbox.left - 1, bbox.bottom + fabsf(bbox.top - bbox.bottom) / 4);
                interfaces.surface->drawLine(bbox.left - 1, bbox.bottom - 1, bbox.left + fabsf(bbox.right - bbox.left) / 4, bbox.bottom - 1);
                interfaces.surface->drawLine(bbox.right + 1, bbox.bottom - 1, bbox.right - fabsf(bbox.right - bbox.left) / 4, bbox.bottom - 1);
                interfaces.surface->drawLine(bbox.right + 1, bbox.bottom - 1, bbox.right + 1, bbox.bottom + fabsf(bbox.top - bbox.bottom) / 4);
                interfaces.surface->drawLine(bbox.left - 1, bbox.top + 1, bbox.left - 1, bbox.top - fabsf(bbox.top - bbox.bottom) / 4);
                interfaces.surface->drawLine(bbox.left - 1, bbox.top + 1, bbox.left + fabsf(bbox.right - bbox.left) / 4, bbox.top + 1);
                interfaces.surface->drawLine(bbox.right + 1, bbox.top + 1, bbox.right - fabsf(bbox.right - bbox.left) / 4, bbox.top + 1);
                interfaces.surface->drawLine(bbox.right + 1, bbox.top + 1, bbox.right + 1, bbox.top - fabsf(bbox.top - bbox.bottom) / 4);
            }
            break;
        case 2:
            for (int i = 0; i < 8; i++) {
                if (!(i & 1))
                    interfaces.surface->drawLine(bbox.vertices[i].x, bbox.vertices[i].y, bbox.vertices[i + 1].x, bbox.vertices[i + 1].y);
                if (!(i & 2))
                    interfaces.surface->drawLine(bbox.vertices[i].x, bbox.vertices[i].y, bbox.vertices[i + 2].x, bbox.vertices[i + 2].y);
                if (!(i & 4))
                    interfaces.surface->drawLine(bbox.vertices[i].x, bbox.vertices[i].y, bbox.vertices[i + 4].x, bbox.vertices[i + 4].y);
            }
            break;
        case 3:
            for (int i = 0; i < 8; i++) {
                if (!(i & 1)) {
                    interfaces.surface->drawLine(bbox.vertices[i].x, bbox.vertices[i].y, bbox.vertices[i].x + (bbox.vertices[i + 1].x - bbox.vertices[i].x) * 0.25f, bbox.vertices[i].y + (bbox.vertices[i + 1].y - bbox.vertices[i].y) * 0.25f);
                    interfaces.surface->drawLine(bbox.vertices[i].x + (bbox.vertices[i + 1].x - bbox.vertices[i].x) * 0.75f, bbox.vertices[i].y + (bbox.vertices[i + 1].y - bbox.vertices[i].y) * 0.75f, bbox.vertices[i + 1].x, bbox.vertices[i + 1].y);
                }
                if (!(i & 2)) {
                    interfaces.surface->drawLine(bbox.vertices[i].x, bbox.vertices[i].y, bbox.vertices[i].x + (bbox.vertices[i + 2].x - bbox.vertices[i].x) * 0.25f, bbox.vertices[i].y + (bbox.vertices[i + 2].y - bbox.vertices[i].y) * 0.25f);
                    interfaces.surface->drawLine(bbox.vertices[i].x + (bbox.vertices[i + 2].x - bbox.vertices[i].x) * 0.75f, bbox.vertices[i].y + (bbox.vertices[i + 2].y - bbox.vertices[i].y) * 0.75f, bbox.vertices[i + 2].x, bbox.vertices[i + 2].y);
                }
                if (!(i & 4)) {
                    interfaces.surface->drawLine(bbox.vertices[i].x, bbox.vertices[i].y, bbox.vertices[i].x + (bbox.vertices[i + 4].x - bbox.vertices[i].x) * 0.25f, bbox.vertices[i].y + (bbox.vertices[i + 4].y - bbox.vertices[i].y) * 0.25f);
                    interfaces.surface->drawLine(bbox.vertices[i].x + (bbox.vertices[i + 4].x - bbox.vertices[i].x) * 0.75f, bbox.vertices[i].y + (bbox.vertices[i + 4].y - bbox.vertices[i].y) * 0.75f, bbox.vertices[i + 4].x, bbox.vertices[i + 4].y);
                }
            }
            break;
        }
    }
}

static void renderPlayerBox(Entity* entity, const Config::Esp::Player& config) noexcept
{
    if (BoundingBox bbox; boundingBox(entity, bbox)) {
        renderBox(entity, bbox, config);

        float drawPositionX = bbox.left - 5;

        if (config.healthBar) {
            static auto gameType{ interfaces.cvar->findVar("game_type") };
            static auto survivalMaxHealth{ interfaces.cvar->findVar("sv_dz_player_max_health") };

            const auto maxHealth{ (std::max)((gameType->getInt() == 6 ? survivalMaxHealth->getInt() : 100), entity->health()) };

            interfaces.surface->setDrawColor(config.healthBarColor, 255);
            interfaces.surface->drawFilledRect(drawPositionX - 3, bbox.bottom + abs(bbox.top - bbox.bottom) * (maxHealth - entity->health()) / static_cast<float>(maxHealth), drawPositionX, bbox.top);
            
            if (config.outline) {
                interfaces.surface->setDrawColor(config.outlineColor, 255);
                interfaces.surface->drawOutlinedRect(drawPositionX - 4, bbox.bottom - 1, drawPositionX + 1, bbox.top + 1);
            }
            drawPositionX -= 7;
        }

        if (config.armorBar) {
            interfaces.surface->setDrawColor(config.armorBarColor, 255);
            interfaces.surface->drawFilledRect(drawPositionX - 3, bbox.bottom + abs(bbox.top - bbox.bottom) * (100.0f - entity->armor()) / 100.0f, drawPositionX, bbox.top);
            
            if (config.outline) {
                interfaces.surface->setDrawColor(config.outlineColor, 255);
                interfaces.surface->drawOutlinedRect(drawPositionX - 4, bbox.bottom - 1, drawPositionX + 1, bbox.top + 1);
            }
            drawPositionX -= 7;
        }

        if (config.name) {
            static PlayerInfo playerInfo;
            if (interfaces.engine->getPlayerInfo(entity->index(), playerInfo)) {
                static wchar_t name[128];
                if (MultiByteToWideChar(CP_UTF8, 0, playerInfo.name, -1, name, 128)) {
                    const auto [width, height] { interfaces.surface->getTextSize(config.font, name) };
                    interfaces.surface->setTextFont(config.font);
                    interfaces.surface->setTextColor(config.nameColor, 255);
                    interfaces.surface->setTextPosition(bbox.left + (fabsf(bbox.right - bbox.left) - width) / 2, bbox.bottom - 5 - height);
                    interfaces.surface->printText(name);
                }
            }
        }

        if (config.activeWeapon) {
            const auto [width, height] { interfaces.surface->getTextSize(config.font, interfaces.localize->find(entity->getActiveWeapon()->getWeaponData()->name)) };
            interfaces.surface->setTextFont(config.font);
            interfaces.surface->setTextColor(config.activeWeaponColor, 255);
            interfaces.surface->setTextPosition(bbox.left + (fabsf(bbox.right - bbox.left) - width) / 2, bbox.bottom - 6 - height);
            interfaces.surface->printText(interfaces.localize->find(entity->getActiveWeapon()->getWeaponData()->name));
        }     

        float drawPositionY = bbox.bottom;

        if (config.health)
            renderPositionedText(config.font, (std::to_wstring(entity->health()) + L" HP").c_str(), config.healthColor, { bbox.right + 5, drawPositionY });

        if (config.armor)
            renderPositionedText(config.font, (std::to_wstring(entity->armor()) + L" AR").c_str(), config.armorColor, { bbox.right + 5, drawPositionY });

        if (config.money)
            renderPositionedText(config.font, (L'$' + std::to_wstring(entity->account())).c_str(), config.moneyColor, { bbox.right + 5, drawPositionY });

        if (const auto localPlayer{ interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer()) }; config.distance)
            renderPositionedText(config.font, (std::wostringstream{ } << std::fixed << std::showpoint << std::setprecision(2) << (entity->getAbsOrigin() - localPlayer->getAbsOrigin()).length() * 0.0254f << L'm').str().c_str(), config.distanceColor, { bbox.right + 5, drawPositionY });
    }
}

static void renderWeaponBox(Entity* entity, const Config::Esp::Weapon& config) noexcept
{
    if (BoundingBox bbox; boundingBox(entity, bbox)) {
        renderBox(entity, bbox, config);

        if (config.name) {
            const auto name{ interfaces.localize->find(entity->getWeaponData()->name) };
            const auto [width, height] { interfaces.surface->getTextSize(config.font, name) };
            interfaces.surface->setTextFont(config.font);
            interfaces.surface->setTextColor(config.nameColor, 255);
            interfaces.surface->setTextPosition(bbox.left + (bbox.right - bbox.left - width) / 2, bbox.top + 5);
            interfaces.surface->printText(name);
        }

        float drawPositionY = bbox.bottom;

        if (const auto localPlayer{ interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer()) }; config.distance)
            renderPositionedText(config.font, (std::wostringstream{ } << std::fixed << std::showpoint << std::setprecision(2) << (entity->getAbsOrigin() - localPlayer->getAbsOrigin()).length() * 0.0254f << L'm').str().c_str(), config.distanceColor, { bbox.right + 5, drawPositionY });
    }
}

static void renderEntityBox(Entity* entity, const Config::Esp::Shared& config, const wchar_t* name) noexcept
{
    if (BoundingBox bbox; boundingBox(entity, bbox)) {
        renderBox(entity, bbox, config);

        if (config.name) {
            const auto [width, height] { interfaces.surface->getTextSize(config.font, name) };
            interfaces.surface->setTextFont(config.font);
            interfaces.surface->setTextColor(config.nameColor, 255);
            interfaces.surface->setTextPosition(bbox.left + (bbox.right - bbox.left - width) / 2, bbox.top + 5);
            interfaces.surface->printText(name);
        }

        float drawPositionY = bbox.bottom;

        if (const auto localPlayer{ interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer()) }; config.distance)
            renderPositionedText(config.font, (std::wostringstream{ } << std::fixed << std::showpoint << std::setprecision(2) << (entity->getAbsOrigin() - localPlayer->getAbsOrigin()).length() * 0.0254f << L'm').str().c_str(), config.distanceColor, { bbox.right + 5, drawPositionY });
    }
}

static constexpr void renderHeadDot(Entity* entity, const Config::Esp::Player& config) noexcept
{
    if (config.headDot) {
        Vector head{ };
        if (worldToScreen(entity->getBonePosition(8), head)) {
            interfaces.surface->setDrawColor(config.headDotColor, 255);
            if (const auto localPlayer{ interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer()) })
                interfaces.surface->drawCircle(head.x, head.y, 0, static_cast<int>(100 / sqrtf((localPlayer->getAbsOrigin() - entity->getAbsOrigin()).length())));
        }
    }
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
    if (config.esp.players[id].enabled && isInRange(entity, config.esp.players[id].maxDistance)) {
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

        for (int i = 1; i <= interfaces.engine->getMaxClients(); i++) {
            auto entity = interfaces.entityList->getEntity(i);
            if (!entity || entity == localPlayer || entity->isDormant()
                || !entity->isAlive())
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
                case ClassId::Dronegun:
                    renderEntityEsp(entity, config.esp.dangerZone[0], interfaces.localize->find("#SFUI_WPNHUD_AutoSentry"));
                    break;
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
