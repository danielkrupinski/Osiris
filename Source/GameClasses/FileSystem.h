#pragma once

#include <CS2/Classes/FileSystem.h>
#include "FileNameSymbolTable.h"
#include "Implementation/FileSystemImpl.h"

struct FileSystem {
    explicit FileSystem(cs2::CBaseFileSystem* thisptr) noexcept
        : thisptr{ thisptr }
    {
    }

    explicit operator bool() const noexcept
    {
        return thisptr != nullptr;
    }

    [[nodiscard]] FileNameSymbolTable fileNames() const noexcept
    {
        return FileNameSymbolTable{FileSystemImpl::instance().fileNamesOffset.of(thisptr).get()};
    }

private:
    cs2::CBaseFileSystem* thisptr;
};
