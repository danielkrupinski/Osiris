#pragma once

#include <Platform/Macros/PlatformSpecific.h>
#include "VirtualMethod.h"

namespace csgo
{

struct BaseFileSystemPOD;

struct BaseFileSystem : GameClass<BaseFileSystem, BaseFileSystemPOD> {
    VIRTUAL_METHOD(int, read, 0, (void* output, int size, void* handle), (output, size, handle))
    VIRTUAL_METHOD(void*, open, 2, (const char* fileName, const char* options, const char* pathID), (fileName, options, pathID))
    VIRTUAL_METHOD(void, close, 3, (void* handle), (handle))
    VIRTUAL_METHOD(unsigned int, size, WIN32_LINUX(7, 6), (void* handle), (handle))
};

}
