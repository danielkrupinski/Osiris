#pragma once

#include <MemoryPatterns/FileSystemPatterns.h>

struct FileSystemImpl {
    explicit FileSystemImpl(const FileSystemPatterns& fileSystemPatterns) noexcept
        : fileNamesOffset{fileSystemPatterns.fileNamesOffset()}
    {
    }

    FileNamesOffset fileNamesOffset;
};
