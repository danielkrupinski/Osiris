#pragma once

#include "../../CS2/Classes/Vector.h"
// Forward declare HookContext if its full definition is not needed here
// or include the full header if it's small / necessary.
// Assuming HookContext.h is the correct path and defines the HookContext type.
#include "../../HookContext/HookContext.h"
// Assuming PlayerPawn.h is the correct path and defines PlayerPawn.
// This might also be templated as PlayerPawn<HookContext>, but the .cpp used PlayerPawn directly.
// For consistency with the .cpp, we'll assume PlayerPawn is a concrete type here.
#include "../../GameClient/Entities/PlayerPawn.h"

// GlobalContext.h might not be needed if HookContext provides all necessary access.
// #include "../../Utils/GlobalContext.h"

class Aimbot {
public:
    void run(HookContext& hookContext);

    // This member stores the last calculated aim angles.
    // It's made public here for simplicity, so Aimbot::run can directly update it.
    // Could be private with a getter if encapsulation is a stronger concern later.
    Vector currentAimAngles;

private:
    // Finds the best target based on criteria (e.g., FOV, distance).
    // Takes the local player's eye position as input.
    // Returns a PlayerPawn object representing the target.
    // Assumes PlayerPawn can be default-constructed or represents a null/invalid state if no target found.
    PlayerPawn findTarget(HookContext& hookContext, const Vector& localPlayerEyePosition);

    // Calculates the pitch and yaw angles required to aim at targetPosition from localPlayerEyePosition.
    // The resulting angles (pitch, yaw, 0 for roll) are stored in outAimAngles.
    void calculateAimAngles(const Vector& localPlayerEyePosition, const Vector& targetPosition, Vector& outAimAngles);
};
