#pragma once

#include <Platform/PlatformSpecific.h>
#include "VirtualMethod.h"

class BaseFileSystem : private VirtualCallable {
public:
    using VirtualCallable::VirtualCallable;

    VIRTUAL_METHOD2(int, read, 0, (void* output, int size, void* handle), (output, size, handle))
    VIRTUAL_METHOD2(void*, open, 2, (const char* fileName, const char* options, const char* pathID), (fileName, options, pathID))
    VIRTUAL_METHOD2(void, close, 3, (void* handle), (handle))
    VIRTUAL_METHOD2(unsigned int, size, WIN32_LINUX(7, 6), (void* handle), (handle))
};
