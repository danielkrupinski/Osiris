#pragma once

#include "ConfigFileOperation.h"
#include <MemoryAllocation/UniquePtr.h>
#include <Platform/PlatformPath.h>

struct ConfigState {
    bool autoSaveEnabled{false};
    bool autoSaveScheduled{false};
    bool loadScheduled{false};
    ConfigFileOperation currentFileOperation{ConfigFileOperation::None};
    UniquePtr<platform::PathCharType[]> pathToConfigDirectory{};
    UniquePtr<platform::PathCharType[]> pathToConfigFile{};
    UniquePtr<platform::PathCharType[]> pathToConfigTempFile{};
};
