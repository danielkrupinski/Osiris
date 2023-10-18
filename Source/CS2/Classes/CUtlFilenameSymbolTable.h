#pragma once

namespace cs2
{

using FileNameHandle_t = void*;

struct CUtlFilenameSymbolTable {
    using String = bool(CUtlFilenameSymbolTable* thisptr, const FileNameHandle_t* handle, char* buf, int buflen);
};

}
