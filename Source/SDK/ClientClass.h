#pragma once

#include <type_traits>

#include "Constants/ClassId.h"
#include <Platform/CallingConventions.h>

struct RecvTable;

namespace csgo::pod { struct Entity; }

struct ClientClass {
    std::add_pointer_t<csgo::pod::Entity* CDECL_CONV(int, int)> createFunction;
    void* createEventFunction;
    char* networkName;
    RecvTable* recvTable;
    ClientClass* next;
    ClassId classId;
};
