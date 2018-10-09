#include "ClientEntityList.h"

template<typename T>
__forceinline constexpr static T callVirtualFunction(void* ppClass, int index)
{
    int* pVTable = *(int**)ppClass;
    int dwAddress = pVTable[index];
    return (T)(dwAddress);
}

BaseEntity* ClientEntityList::getClientEntity(int index)
{
    return callVirtualFunction<BaseEntity*(__thiscall*)(void*, int)>(this, 3)(this, index);
}
