#pragma once

#include "VirtualMethod.h"

class NetworkStringTable {
public:
    VIRTUAL_METHOD_V(int, addString, 8, (bool isServer, const char* value, int length = -1, const void* userdata = nullptr), (this, isServer, value, length, userdata))
};

class NetworkStringTableContainer {
public:
    VIRTUAL_METHOD_V(NetworkStringTable*, findTable, 3, (const char* name), (this, name))
};
