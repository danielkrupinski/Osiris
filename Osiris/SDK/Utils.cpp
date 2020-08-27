#include "GlobalVars.h"
#include "../Memory.h"
#include "Utils.h"

std::tuple<float, float, float> rainbowColor(float speed) noexcept
{
    return std::make_tuple(std::sin(speed * memory->globalVars->realtime) * 0.5f + 0.5f,
                           std::sin(speed * memory->globalVars->realtime + static_cast<float>(2 * M_PI / 3)) * 0.5f + 0.5f,
                           std::sin(speed * memory->globalVars->realtime + static_cast<float>(4 * M_PI / 3)) * 0.5f + 0.5f);
}
