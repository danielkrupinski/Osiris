#pragma once

#include <Platform/Macros/IsPlatform.h>
#include <Utils/Pad.h>

namespace cs2
{

#if IS_WIN64()
struct CEntityClassInfo {
    const void* unknown;
    const char* name;
};

struct CEntityClass {
    PAD(88);
    const CEntityClassInfo* info;
};
#else
struct CEntityClass {};
#endif

}
