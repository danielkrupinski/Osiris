#pragma once

#include <cstdint>

#include <CS2/Classes/FileSystem.h>
#include <Utils/FieldOffset.h>
#include <Utils/StrongTypeAlias.h>

template <typename FieldType, typename OffsetType>
using FileSystemOffset = FieldOffset<cs2::CBaseFileSystem, FieldType, OffsetType>;

STRONG_TYPE_ALIAS(FileSystemPointer, cs2::CBaseFileSystem**);
STRONG_TYPE_ALIAS(FileNamesOffset, FileSystemOffset<cs2::CBaseFileSystem::m_FileNames, std::int32_t>);
