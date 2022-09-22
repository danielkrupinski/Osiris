#pragma once

struct UserCmd;

class GlobalContext {
public:
    bool createMoveHook(float inputSampleTime, UserCmd* cmd);
};

inline GlobalContext globalContext;
