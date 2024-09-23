#pragma once

#include <CS2/Classes/CUtlFilenameSymbolTable.h>
#include <GameDLLs/Tier0Dll.h>

struct FileNameSymbolTableDeps {
    FileNameSymbolTableDeps() = default;

    explicit FileNameSymbolTableDeps(Tier0Dll tier0Dll) noexcept
        : string{ tier0Dll.filenameSymbolTableString() }
    {
    }

    [[nodiscard]] static const FileNameSymbolTableDeps& instance() noexcept;

    cs2::CUtlFilenameSymbolTable::String* string;
};
