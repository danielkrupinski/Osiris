#pragma once

class Surface {
public:
    void UnlockCursor()
    {
        callVirtualFunction<void(__thiscall*)(void*)>(this, 66)(this);
    }

    void LockCursor()
    {
        callVirtualFunction<void(__thiscall*)(void*)>(this, 67)(this);
    }
};
