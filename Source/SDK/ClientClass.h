#pragma once

#include <type_traits>

#include "Constants/ClassId.h"
#include "Platform.h"

struct RecvTable;

struct ClientClass {
    std::add_pointer_t<std::uintptr_t CDECL_CONV(int, int)> createFunction;
    void* createEventFunction;
    char* networkName;
    RecvTable* recvTable;
    ClientClass* next;
    ClassId classId;
};
