#include "../Config.h"
#include "Visuals.h"

bool Visuals::removeSleeves(const std::string_view modelName) noexcept
{
    return config.visuals.noSleeves && (modelName.find("sleeve") != std::string_view::npos);
}

void Visuals::thirdperson()
{

}
