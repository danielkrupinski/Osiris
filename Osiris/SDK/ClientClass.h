#pragma once

#include "ClassId.h"

struct RecvTable;

struct ClientClass final {
    void* createFunction;
    void* createEventFunction;
    char* networkName;
    RecvTable* recvTable;
    ClientClass* next;
    ClassId classId;
};
