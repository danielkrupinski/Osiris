#pragma once

#include <CS2/Classes/FileSystem.h>
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

    [[nodiscard]] cs2::CBaseFileSystem::m_FileNames* fileNames() const noexcept
    {
        return FileSystemImpl::instance().fileNamesOffset.of(thisptr).get();
    }

private:
    cs2::CBaseFileSystem* thisptr;
};
