#pragma once

#include <CS2/Classes/FileSystem.h>
#include "FileNameSymbolTable.h"
#include "Implementation/FileSystemImpl.h"

struct FileSystem {
    explicit FileSystem(cs2::CBaseFileSystem& thisptr, const FileSystemImpl& impl) noexcept
        : thisptr{thisptr}
        , impl{impl}
    {
    }

    [[nodiscard]] FileNameSymbolTable fileNames() const noexcept
    {
        return FileNameSymbolTable{impl.fileNamesOffset.of(&thisptr).get()};
    }

private:
    cs2::CBaseFileSystem& thisptr;
    const FileSystemImpl& impl;
};
