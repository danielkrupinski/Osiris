#pragma once

#include <CS2/Classes/FileSystem.h>
#include "FileNameSymbolTable.h"
#include <GameDependencies/FileSystemDeps.h>

struct FileSystem {
    explicit FileSystem(cs2::CBaseFileSystem& thisptr, const FileSystemDeps& deps) noexcept
        : thisptr{thisptr}
        , deps{deps}
    {
    }

    [[nodiscard]] FileNameSymbolTable fileNames() const noexcept
    {
        return FileNameSymbolTable{deps.fileNamesOffset.of(&thisptr).get()};
    }

private:
    cs2::CBaseFileSystem& thisptr;
    const FileSystemDeps& deps;
};
