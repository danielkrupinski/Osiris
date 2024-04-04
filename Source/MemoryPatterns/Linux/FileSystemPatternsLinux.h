#pragma once

#include <MemoryPatterns/FileSystemPatterns.h>
#include <MemorySearch/BytePatternLiteral.h>

inline cs2::CBaseFileSystem** FileSystemPatterns::fileSystem() const noexcept
{
    return soundSystemPatternFinder("48 8D 05 ? ? ? ? 48 89 DA 4C 8B 00"_pat).add(3).abs().as<cs2::CBaseFileSystem**>();
}

inline FileNamesOffset FileSystemPatterns::fileNamesOffset() const noexcept
{
    return fileSystemPatternFinder("48 8D BB ? ? ? ? E8 ? ? ? ? 8B 83 ? ? ? ? 89 C2"_pat).add(3).readOffset<FileNamesOffset>();
}
