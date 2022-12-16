#pragma once

#include <type_traits>

#include "Constants/ClassId.h"
#include <Platform/Macros/CallingConventions.h>

namespace csgo
{

struct RecvTable;

namespace pod { struct Entity; }

struct ClientClass {
    std::add_pointer_t<pod::Entity* CDECL_CONV(int, int)> createFunction;
    void* createEventFunction;
    char* networkName;
    RecvTable* recvTable;
    ClientClass* next;
    ClassId classId;
};

}
