#pragma once

#include "ClassId.h"

struct ClientClass {
    void* createFunction;
    void* createEventFunction;
    char* networkName;
    void* recvTable; // RecvTable*
    ClientClass* next;
    ClassId classId;
};
