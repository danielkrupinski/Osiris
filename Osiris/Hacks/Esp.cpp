#include "Esp.h"
#include "../Config.h"
#include "../Interfaces.h"
#include "../SDK/Entity.h"
#include "../SDK/Vector.h"

static bool worldToScreen(const Vector& in, Vector& out) noexcept
{
    static auto matrix = interfaces.engine->worldToScreenMatrix();
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

static void renderSnaplines(Entity* entity, decltype(config.esp[0])& config) noexcept
{
    if (config.snaplines) {
        Vector position;
        if (worldToScreen(entity->getProperty<Vector>("m_vecOrigin"), position)) {
            const auto [width, height] = interfaces.surface->getScreenSize();
            interfaces.surface->setDrawColor(config.snaplinesColor, 255);
            interfaces.surface->drawLine(width / 2, height, static_cast<int>(position.x), static_cast<int>(position.y));
        }
    }
}

static void renderBox(Entity* entity, decltype(config.esp[0])& config) noexcept
{
    if (config.box) {
        Vector bottom, top;
        Vector head{ entity->getBonePosition(8) };
        head.z += 10.0f;
        if (worldToScreen(entity->getProperty<Vector>("m_vecOrigin"), bottom) && worldToScreen(head, top)) {
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
        Vector head{ entity->getBonePosition(8) }, top;
        head.z += 10.0f;
        if (worldToScreen(head, top)) {
            static PlayerInfo playerInfo;
            if (interfaces.engine->getPlayerInfo(entityIndex, playerInfo)) {
                static wchar_t name[128];
                if (MultiByteToWideChar(CP_UTF8, 0, playerInfo.name, -1, name, 128)) {
                    static unsigned font = interfaces.surface->createFont();
                    static bool init = interfaces.surface->setFontGlyphSet(font, "Tahoma", 14, 700, 0, 0, 128);
                    const auto [width, height] = interfaces.surface->getTextSize(font, name);
                    interfaces.surface->setTextFont(font);
                    interfaces.surface->setTextColor(config.nameColor, 255);
                    interfaces.surface->setTextPosition(top.x - width / 2, top.y + 5 - height);
                    interfaces.surface->printText(name);
                }
            }
        }
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
                if (entity->isVisible()) {
                    if (config.esp[0].enabled) {
                        renderSnaplines(entity, config.esp[0]);
                        renderBox(entity, config.esp[0]);
                    }
                } else {
                    if (config.esp[1].enabled) {
                        renderSnaplines(entity, config.esp[1]);
                        renderBox(entity, config.esp[1]);
                    }
                }
            } else {
                if (entity->isVisible()) {
                    if (config.esp[2].enabled) {
                        renderSnaplines(entity, config.esp[2]);
                        renderBox(entity, config.esp[2]);
                    }
                } else {
                    if (config.esp[3].enabled) {
                        renderSnaplines(entity, config.esp[3]);
                        renderBox(entity, config.esp[3]);
                    }
                }
            }
        }
    }
}
