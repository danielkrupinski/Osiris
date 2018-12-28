#include "Aimbot.h"
#include "../Interfaces.h"
#include "../Memory.h"
#include "../SDK/UserCmd.h"
#include "../SDK/Vector.h"

static Vector calculateAngleBetween(Vector source, Vector destination)
{
    Vector delta = source - destination;
    float hyp = delta.length2D();

    Vector angles;
    angles.y = std::atanf(delta.y / delta.x) * 57.2957795131f;
    angles.x = std::atanf(-delta.z / hyp) * -57.2957795131f;
    angles.z = 0.0f;

    if (delta.x >= 0.0f)
        angles.y += 180.0f;

    return angles;
}

static int findTarget(UserCmd* cmd, Vector& position)
{
    for (int i = 1; i < interfaces.engine->getMaxClients(); i++) {
        auto entity = interfaces.entityList->getClientEntity(i);
        if (!entity || entity->isDormant() || !entity->isAlive() || !entity->isEnemy())
            continue;
    }
}

void Aimbot::run()
{

}
