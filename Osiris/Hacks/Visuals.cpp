#include "../Config.h"
#include "../Memory.h"
#include "Visuals.h"

void Visuals::thirdperson()
{
    static bool isInThirdperson{ true };
    if (GetAsyncKeyState(config.visuals.thirdpersonKey))
        isInThirdperson = !isInThirdperson;
    if (memory.input->isCameraInThirdPerson =
        (config.visuals.thirdperson && isInThirdperson
            && interfaces.entityList->getClientEntity(interfaces.engine->getLocalPlayer())->isAlive()))
        memory.input->cameraOffset.z = static_cast<float>(config.visuals.thirdpersonDistance);
}
