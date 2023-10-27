#pragma once

#include <CS2/Classes/CUtlFilenameSymbolTable.h>
#include <GameDLLs/Tier0Dll.h>

struct FileNameSymbolTableImpl {
    explicit FileNameSymbolTableImpl(Tier0Dll tier0Dll) noexcept
        : string{ tier0Dll.filenameSymbolTableString() }
    {
    }

    [[nodiscard]] static const FileNameSymbolTableImpl& instance() noexcept;

    cs2::CUtlFilenameSymbolTable::String* string;
};
