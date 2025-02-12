#pragma once

#include <CS2/Classes/CUtlFilenameSymbolTable.h>
#include <GameClient/DLLs/Tier0Dll.h>

struct FileNameSymbolTableState {
    explicit FileNameSymbolTableState(Tier0Dll tier0Dll) noexcept
        : string{tier0Dll.filenameSymbolTableString()}
    {
    }

    cs2::CUtlFilenameSymbolTable::String* string;
};
