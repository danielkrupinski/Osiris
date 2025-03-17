#pragma once

#include "ConfigFileOperation.h"
#include <MemoryAllocation/UniquePtr.h>
#include <Platform/PlatformPath.h>

#include "ConfigVariables.h"

struct ConfigState {
    bool autoSaveScheduled{false};
    bool loadScheduled{false};
    ConfigFileOperation currentFileOperation{ConfigFileOperation::None};
    UniquePtr<platform::PathCharType[]> pathToConfigDirectory{};
    UniquePtr<platform::PathCharType[]> pathToConfigFile{};
    UniquePtr<platform::PathCharType[]> pathToConfigTempFile{};
    ConfigVariables configVariables{};
};
