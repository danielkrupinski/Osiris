#pragma once

struct TriggerbotState {
    bool firing{false};
    float lastFireTime{0.0f};     // engine curtime of last shot (cooldown gate)
};
