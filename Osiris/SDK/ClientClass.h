#pragma once

#include "ClassId.h"
#include "Recv.h"

struct ClientClass final {
    void* createFunction;
    void* createEventFunction;
    char* networkName;
    RecvTable* recvTable;
    ClientClass* next;
    ClassId classId;
};
