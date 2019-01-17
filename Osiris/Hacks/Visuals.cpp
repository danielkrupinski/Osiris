#include "../Config.h"
#include "../Memory.h"
#include "Visuals.h"

bool Visuals::removeHands(const std::string_view modelName) noexcept
{
    return config.visuals.noHands && (modelName.find("arms") != std::string_view::npos);
}

bool Visuals::removeSleeves(const std::string_view modelName) noexcept
{
    return config.visuals.noSleeves && (modelName.find("sleeve") != std::string_view::npos);
}

void Visuals::thirdperson()
{
    if (memory.input->isCameraInThirdPerson = config.visuals.thirdperson)
        memory.input->cameraOffset.z = static_cast<float>(config.visuals.thirdpersonDistance);
}
