#pragma once

#include <Config.h>
#include "../Memory.h"

struct UserCmd;
struct Vector;
class ClientInterfaces;
class EngineInterfaces;

class Aimbot {
public:
    static Vector calculateRelativeAngle(const Vector& source, const Vector& destination, const Vector& viewAngles) noexcept;
    void run(const EngineInterfaces& engineInterfaces, const ClientInterfaces& clientInterfaces, const OtherInterfaces& interfaces, const Config& config, const Memory& memory, UserCmd*) noexcept;

    void updateInput(const Config& config) noexcept;
};
