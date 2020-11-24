#pragma once

#include <type_traits>

#include "ClassId.h"
#include "Platform.h"

class Entity;
struct RecvTable;

struct ClientClass {
    std::add_pointer_t<Entity* __CDECL(int, int)> createFunction;
    void* createEventFunction;
    char* networkName;
    RecvTable* recvTable;
    ClientClass* next;
    ClassId classId;
};
