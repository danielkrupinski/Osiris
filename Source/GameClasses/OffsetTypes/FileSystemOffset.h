#pragma once

#include <CS2/Classes/FileSystem.h>
#include <Utils/FieldOffset.h>

template <typename FieldType, typename OffsetType>
using FileSystemOffset = FieldOffset<cs2::CBaseFileSystem, FieldType, OffsetType>;
