#pragma once

#include "VirtualMethod.h"

class NetworkStringTable {
public:
    VIRTUAL_METHOD(int, addString, 8, (bool isServer, const char* value, int length = -1, const void* userdata = nullptr), (this, isServer, value, length, userdata))
};
