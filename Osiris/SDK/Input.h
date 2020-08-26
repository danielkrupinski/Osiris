#pragma once

#include "Vector.h"

template<typename FuncType>
__forceinline static FuncType qCallVFunction(void* ppClass, int index)
{
    int* pVTable = *(int**)ppClass;
    int dwAddress = pVTable[index];
    return (FuncType)(dwAddress);
}

class Input {
public:
    std::byte pad[12];
    bool isTrackIRAvailable;
    bool isMouseInitialized;
    bool isMouseActive;
    std::byte pad1[158];
    bool isCameraInThirdPerson;
    std::byte pad2;
    Vector cameraOffset;

    UserCmd* GetUserCmd(int nSlot, int sequence_number)
    {
        using OriginalFn = UserCmd * (__thiscall*)(void*, int, int);
        return qCallVFunction<OriginalFn>(this, 8)(this, nSlot, sequence_number);
    }
};
