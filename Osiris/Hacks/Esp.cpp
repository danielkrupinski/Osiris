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

static constexpr void renderBox(Entity* entity, const decltype(config.esp[0])& config) noexcept
{
    if (config.box) {
        Vector bottom{ }, top{ }, head{ entity->getBonePosition(8) };
        head.z += 10.0f;
        if (worldToScreen(entity->getAbsOrigin(), bottom) && worldToScreen(head, top)) {
            interfaces.surface->setDrawColor(config.boxColor, 255);
            float width = abs(top.y - bottom.y) * 0.3f;
            interfaces.surface->drawOutlinedRect(bottom.x - width, top.y, bottom.x + width, bottom.y);
            interfaces.surface->setDrawColor(0, 0, 0, 255);
            interfaces.surface->drawOutlinedRect(bottom.x - width + 1, top.y + 1, bottom.x + width - 1, bottom.y - 1);
            interfaces.surface->drawOutlinedRect(bottom.x - width - 1, top.y - 1, bottom.x + width + 1, bottom.y + 1);
        }
    }
}

static void renderName(int entityIndex, const decltype(config.esp[0])& config) noexcept
{
    if (config.name) {
        Entity* entity = interfaces.entityList->getEntity(entityIndex);
        Vector head{ entity->getBonePosition(8) }, bottom, top;
        head.z += 10.0f;
        if (worldToScreen(head, top)) {
            static PlayerInfo playerInfo;
            if (worldToScreen(entity->getAbsOrigin(), bottom) && interfaces.engine->getPlayerInfo(entityIndex, playerInfo)) {
                static wchar_t name[128];
                if (MultiByteToWideChar(CP_UTF8, 0, playerInfo.name, -1, name, 128)) {
                    static unsigned font = interfaces.surface->createFont();
                    static bool init = interfaces.surface->setFontGlyphSet(font, "Tahoma", 12, 700, 0, 0, 128);
                    const auto [width, height] = interfaces.surface->getTextSize(font, name);
                    interfaces.surface->setTextFont(font);
                    interfaces.surface->setTextColor(config.nameColor, 255);
                    interfaces.surface->setTextPosition(bottom.x - width / 2, top.y - 5 - height);
                    interfaces.surface->printText(name);
                }
            }
        }
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
    ALLIES_VISIBLE = 0,
    ALLIES_OCCLUDED,
    ENEMIES_VISIBLE,
    ENEMIES_OCCLUDED
};

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
                if (entity->isVisible()) {
                    if (config.esp[ALLIES_VISIBLE].enabled) {
                        renderSnaplines(entity, config.esp[ALLIES_VISIBLE]);
                        renderBox(entity, config.esp[ALLIES_VISIBLE]);
                        renderName(i, config.esp[ALLIES_VISIBLE]);
                        renderHeadDot(entity, config.esp[ALLIES_VISIBLE]);
                    }
                } else {
                    if (config.esp[ALLIES_OCCLUDED].enabled) {
                        renderSnaplines(entity, config.esp[ALLIES_OCCLUDED]);
                        renderBox(entity, config.esp[ALLIES_OCCLUDED]);
                        renderName(i, config.esp[ALLIES_OCCLUDED]);
                        renderHeadDot(entity, config.esp[ALLIES_OCCLUDED]);
                    }
                }
            } else {
                if (entity->isVisible()) {
                    if (config.esp[ENEMIES_VISIBLE].enabled) {
                        renderSnaplines(entity, config.esp[ENEMIES_VISIBLE]);
                        renderBox(entity, config.esp[ENEMIES_VISIBLE]);
                        renderName(i, config.esp[ENEMIES_VISIBLE]);
                        renderHeadDot(entity, config.esp[ENEMIES_VISIBLE]);
                    }
                } else {
                    if (config.esp[ENEMIES_OCCLUDED].enabled) {
                        renderSnaplines(entity, config.esp[ENEMIES_OCCLUDED]);
                        renderBox(entity, config.esp[ENEMIES_OCCLUDED]);
                        renderName(i, config.esp[ENEMIES_OCCLUDED]);
                        renderHeadDot(entity, config.esp[ENEMIES_OCCLUDED]);
                    }
                }
            }
        }
    }
}
