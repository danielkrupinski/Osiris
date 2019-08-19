#pragma once

#include <type_traits>
#include "ClassId.h"

class Entity;
struct RecvTable;

struct ClientClass {
    std::add_pointer_t<Entity* (int, int)> createFunction;
    void* createEventFunction;
    char* networkName;
    RecvTable* recvTable;
    ClientClass* next;
    ClassId classId;
};
