#pragma once

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string_view>

#include <MemorySearch/BytePattern.h>
#include <MemorySearch/HybridPatternFinder.h>
#include <Utils/MemorySection.h>

#include "RTTI/LinuxRttiTypeDescriptor.h"
#include "RTTI/LinuxRttiTypeDescriptorFinder.h"

#include "LinuxVmt.h"
#include "LinuxVmtFinderParams.h"

class LinuxVmtFinder {
public:
    LinuxVmtFinder(const LinuxVmtFinderParams& params) noexcept
        : rodataSection{params.rodataSection}
        , dataRelRoSection{params.dataRelRoSection}
    {
    }

    [[nodiscard]] const void* findVmt(std::string_view mangledTypeName) const noexcept
    {
        const auto typeDescriptor{findTypeDescriptor(mangledTypeName)};
        if (!typeDescriptor) {
            assert(typeDescriptor != nullptr && "Failed to find type descriptor!");
            return nullptr;
        }

        HybridPatternFinder typeDescriptorCrossReferenceFinder{dataRelRoSection.raw(), BytePattern::ofObject(typeDescriptor)};

        auto typeDescriptorReference{typeDescriptorCrossReferenceFinder.findNextOccurrence()};
        while (typeDescriptorReference && (!canPrecedeVmt(reinterpret_cast<std::uintptr_t>(typeDescriptorReference)) || !isPrecedingVmtOfCompleteClass(typeDescriptorReference)))
            typeDescriptorReference = typeDescriptorCrossReferenceFinder.findNextOccurrence();

        if (typeDescriptorReference)
            return typeDescriptorReference - LinuxVmt::offsetOfTypeDescriptorPointer + LinuxVmt::offsetOfFirstMethodPointer;

        assert(false && "Failed to find VMT!");
        return nullptr;
    }

private:
    [[nodiscard]] const LinuxRttiTypeDescriptor* findTypeDescriptor(std::string_view mangledTypeName) const noexcept
    {
        return LinuxRttiTypeDescriptorFinder{rodataSection, dataRelRoSection}.findTypeDescriptor(mangledTypeName);
    }

    [[nodiscard]] bool canPrecedeVmt(std::uintptr_t typeDescriptorReference) const noexcept
    {
        return dataRelRoSection.offsetOf(typeDescriptorReference) >= LinuxVmt::offsetOfTypeDescriptorPointer;
    }

    [[nodiscard]] bool isPrecedingVmtOfCompleteClass(const std::byte* typeDescriptorReference) const noexcept
    {
        std::size_t offsetFromTop;
        std::memcpy(&offsetFromTop, typeDescriptorReference - LinuxVmt::offsetOfTypeDescriptorPointer + LinuxVmt::offsetOfOffsetFromTop, sizeof(std::size_t));
        return offsetFromTop == 0;
    }

    MemorySection rodataSection;
    MemorySection dataRelRoSection;
};
