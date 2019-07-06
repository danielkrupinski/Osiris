#define _USE_MATH_DEFINES
#include <cmath>
#include "Esp.h"
#include "../Config.h"
#include "../Interfaces.h"
#include "../SDK/Entity.h"
#include "../SDK/Vector.h"
#include "../SDK/Surface.h"

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

static constexpr void renderSnaplines(Entity* entity, const decltype(config.esp[0])& config) noexcept
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

static void renderEyeTraces(Entity* entity, const decltype(config.esp[0])& config) noexcept
{
    if (config.eyeTraces) {
        constexpr auto degreesToRadians = [](float degrees) noexcept { return degrees * static_cast<float>(M_PI) / 180; };
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

static constexpr void renderPositionedText(unsigned font, const wchar_t* text, float color[3], std::pair<float, float&> position) noexcept
{
    interfaces.surface->setTextFont(font);
    interfaces.surface->setTextColor(color, 255);
    interfaces.surface->setTextPosition(position.first, position.second);
    position.second += interfaces.surface->getTextSize(font, text).second;
    interfaces.surface->printText(text);
}

static void renderBox(Entity* entity, const decltype(config.esp[0])& config) noexcept
{
    Vector bottom{ }, top{ }, head{ entity->getBonePosition(8) };
    head.z += 10.0f;
    if (worldToScreen(entity->getAbsOrigin(), bottom) && worldToScreen(head, top)) {
        const float boxWidth = abs(top.y - bottom.y) * 0.3f;

        if (config.box) {
            interfaces.surface->setDrawColor(config.boxColor, 255);
            interfaces.surface->drawOutlinedRect(bottom.x - boxWidth, top.y, bottom.x + boxWidth, bottom.y);
            interfaces.surface->setDrawColor(0, 0, 0, 255);
            interfaces.surface->drawOutlinedRect(bottom.x - boxWidth + 1, top.y + 1, bottom.x + boxWidth - 1, bottom.y - 1);
            interfaces.surface->drawOutlinedRect(bottom.x - boxWidth - 1, top.y - 1, bottom.x + boxWidth + 1, bottom.y + 1);
        }

        float drawPositionX = bottom.x - boxWidth - 5;

        if (config.healthBar) {
            interfaces.surface->setDrawColor(config.healthBarColor, 255);
            interfaces.surface->drawFilledRect(drawPositionX - 3, top.y + abs(top.y - bottom.y) * (100.0f - entity->health()) / 100.0f, drawPositionX, bottom.y);
            interfaces.surface->setDrawColor(0, 0, 0, 255);
            interfaces.surface->drawOutlinedRect(drawPositionX - 4, top.y - 1, drawPositionX + 1, bottom.y + 1);
            drawPositionX -= 7;
        }

        if (config.armorBar) {
            interfaces.surface->setDrawColor(config.armorBarColor, 255);
            interfaces.surface->drawFilledRect(drawPositionX - 3, top.y + abs(top.y - bottom.y) * (100.0f - entity->armor()) / 100.0f, drawPositionX, bottom.y);
            interfaces.surface->setDrawColor(0, 0, 0, 255);
            interfaces.surface->drawOutlinedRect(drawPositionX - 4, top.y - 1, drawPositionX + 1, bottom.y + 1);
            drawPositionX -= 7;
        }

        if (config.name) {
            static PlayerInfo playerInfo;
            if (interfaces.engine->getPlayerInfo(entity->index(), playerInfo)) {
                static wchar_t name[128];
                if (MultiByteToWideChar(CP_UTF8, 0, playerInfo.name, -1, name, 128)) {
                    const auto [width, height] = interfaces.surface->getTextSize(config.font, name);
                    interfaces.surface->setTextFont(config.font);
                    interfaces.surface->setTextColor(config.nameColor, 255);
                    interfaces.surface->setTextPosition(bottom.x - width / 2, top.y - 5 - height);
                    interfaces.surface->printText(name);
                }
            }
        }

        float drawPositionY = top.y;

        if (config.health)
            renderPositionedText(config.font, (std::to_wstring(entity->health()) + L" HP").c_str(), config.healthColor, { bottom.x + boxWidth + 5, drawPositionY });

        if (config.armor)
            renderPositionedText(config.font, (std::to_wstring(entity->armor()) + L" AR").c_str(), config.armorColor, { bottom.x + boxWidth + 5, drawPositionY });

        if (config.money)
            renderPositionedText(config.font, (L'$' + std::to_wstring(entity->getProperty<int>("m_iAccount"))).c_str(), config.moneyColor, { bottom.x + boxWidth + 5, drawPositionY });
    }
}

static constexpr void renderHeadDot(Entity* entity, const decltype(config.esp[0])& config) noexcept
{
    if (config.headDot) {
        Vector head{ };
        if (worldToScreen(entity->getBonePosition(8), head)) {
            interfaces.surface->setDrawColor(config.headDotColor, 255);
            for (int i = 1; i <= 3; i++)
                interfaces.surface->drawOutlinedCircle(head.x, head.y, i, 100);
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

static constexpr bool renderEspForEntity(Entity* entity, EspId id) noexcept
{
    if (config.esp[id].enabled) {
        renderSnaplines(entity, config.esp[id]);
        renderEyeTraces(entity, config.esp[id]);
        renderBox(entity, config.esp[id]);
        renderHeadDot(entity, config.esp[id]);
    }
    return config.esp[id].enabled;
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
                if (!renderEspForEntity(entity, ALLIES_ALL)) {
                    if (entity->isVisible())
                        renderEspForEntity(entity, ALLIES_VISIBLE);
                    else
                        renderEspForEntity(entity, ALLIES_OCCLUDED);
                }
            } else if (!renderEspForEntity(entity, ENEMIES_ALL)) {
                if (entity->isVisible())
                    renderEspForEntity(entity, ENEMIES_VISIBLE);
                else
                    renderEspForEntity(entity, ENEMIES_OCCLUDED);
            }
        }
    }
}
