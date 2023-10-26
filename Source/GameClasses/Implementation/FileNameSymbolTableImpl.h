#pragma once

#include <CS2/Classes/CUtlFilenameSymbolTable.h>
#include <GameDLLs/Tier0Dll.h>

struct FileNameSymbolTableImpl {
    [[nodiscard]] static const FileNameSymbolTableImpl& instance() noexcept;

    cs2::CUtlFilenameSymbolTable::String* string{ Tier0Dll{}.filenameSymbolTableString() };
};
