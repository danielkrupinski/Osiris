#pragma once

#include <CS2/Classes/FileSystem.h>
#include "FileNameSymbolTable.h"
#include <GameDependencies/FileSystemDeps.h>

template <typename HookContext>
class FileSystem {
public:
    explicit FileSystem(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    [[nodiscard]] decltype(auto) fileNames() const noexcept
    {
        return hookContext.template make<FileNameSymbolTable>(deps().fileNamesOffset.of(fileSystem()).get());
    }

private:
    [[nodiscard]] cs2::CBaseFileSystem* fileSystem() const noexcept
    {
        if (const auto fileSystemPointer = hookContext.gameDependencies().fileSystem)
            return *fileSystemPointer;
        return nullptr;
    }

    [[nodiscard]] const auto& deps() const noexcept
    {
        return hookContext.gameDependencies().fileSystemDeps;
    }

    HookContext& hookContext;
};
