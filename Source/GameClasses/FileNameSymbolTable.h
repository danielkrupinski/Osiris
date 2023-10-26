#pragma once

#include <span>

#include <CS2/Classes/CUtlFilenameSymbolTable.h>
#include "Implementation/FileNameSymbolTableImpl.h"

struct FileNameSymbolTable {
    explicit FileNameSymbolTable(cs2::CUtlFilenameSymbolTable* thisptr) noexcept
        : thisptr{ thisptr }
    {
    }

    explicit operator bool() const noexcept
    {
        return thisptr != nullptr;
    }

    void getString(cs2::FileNameHandle_t handle, std::span<char> buffer) const noexcept
    {
        if (FileNameSymbolTableImpl::instance().string)
            FileNameSymbolTableImpl::instance().string(thisptr, &handle, buffer.data(), static_cast<int>(buffer.size()));
    }

private:
    cs2::CUtlFilenameSymbolTable* thisptr;
};
