#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>

#include <MemorySearch/BytePattern.h>
#include <MemorySearch/HybridPatternFinder.h>
#include <Utils/MemorySection.h>

#include "LinuxRttiTypeDescriptor.h"

class LinuxRttiTypeDescriptorFinder {
public:
    LinuxRttiTypeDescriptorFinder(MemorySection rodataSection, MemorySection dataRelRoSection) noexcept
        : rodataSection{rodataSection}
        , dataRelRoSection{dataRelRoSection}
    {
    }

    [[nodiscard]] const LinuxRttiTypeDescriptor* findTypeDescriptor(std::string_view mangledTypeName) const noexcept
    {
        if (const auto typeDescriptorName{findTypeDescriptorName(mangledTypeName)}; isPartOfTypeDescriptor(typeDescriptorName))
            return toTypeDescriptorPointer(typeDescriptorName);
        return nullptr;
    }

private:
    [[nodiscard]] const std::byte* findTypeDescriptorName(std::string_view mangledTypeName) const noexcept
    {
        HybridPatternFinder typeNameFinder{rodataSection.raw(), BytePattern{mangledTypeName}};
        auto typeNameAddress = typeNameFinder.findNextOccurrence();
        while (typeNameAddress) {
            const auto typeNameReference = HybridPatternFinder{dataRelRoSection.raw(), BytePattern::ofObject(typeNameAddress)}.findNextOccurrence();
            if (typeNameReference && reinterpret_cast<std::uintptr_t>(typeNameReference) % alignof(void*) == 0)
                return typeNameReference;
            typeNameAddress = typeNameFinder.findNextOccurrence();
        }
        return nullptr;
    }

    [[nodiscard]] bool isPartOfTypeDescriptor(const std::byte* typeDescriptorName) const noexcept
    {
        return typeDescriptorName && dataRelRoSection.offsetOf(reinterpret_cast<std::uintptr_t>(typeDescriptorName)) >= LinuxRttiTypeDescriptor::kOffsetOfNamePointer;
    }

    [[nodiscard]] static const LinuxRttiTypeDescriptor* toTypeDescriptorPointer(const std::byte* typeDescriptorName) noexcept
    {
        return reinterpret_cast<const LinuxRttiTypeDescriptor*>(typeDescriptorName - LinuxRttiTypeDescriptor::kOffsetOfNamePointer);
    }

    MemorySection rodataSection;
    MemorySection dataRelRoSection;
};
