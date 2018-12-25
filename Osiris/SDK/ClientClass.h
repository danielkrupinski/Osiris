#pragma once

#include "ClassId.h"

class ClientClass {
public:
    void* createFunction;
    void* createEventFunction;
    char* networkName;
    void* recvTable; // RecvTable*
    ClientClass* next;
    ClassId classId;
};
