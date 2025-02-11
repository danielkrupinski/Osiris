#pragma once

#include <CS2/Classes/FileSystem.h>
#include <MemoryPatterns/PatternTypes/FileSystemPatternTypes.h>
#include "FileNameSymbolTable.h"

template <typename HookContext>
class FileSystem {
public:
    explicit FileSystem(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    [[nodiscard]] decltype(auto) fileNames() const noexcept
    {
        return hookContext.template make<FileNameSymbolTable>(hookContext.fileSystemPatternSearchResults().template get<FileNamesOffset>().of(fileSystem()).get());
    }

private:
    [[nodiscard]] cs2::CBaseFileSystem* fileSystem() const noexcept
    {
        if (const auto fileSystemPointer = hookContext.soundSystemPatternSearchResults().template get<FileSystemPointer>())
            return *fileSystemPointer;
        return nullptr;
    }

    HookContext& hookContext;
};
