#pragma once

#include <cstdint>
#include <string_view>

#include <MemorySearch/BinaryBytePattern.h>
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
        if (const auto typeDescriptorName{findTypeDescriptorName(findTypeName(mangledTypeName))}; isPartOfTypeDescriptor(typeDescriptorName))
            return toTypeDescriptorPointer(typeDescriptorName);
        return nullptr;
    }

private:
    [[nodiscard]] const std::byte* findTypeName(std::string_view mangledTypeName) const noexcept
    {
        return HybridPatternFinder{rodataSection.raw(), BytePattern{mangledTypeName}}.findNextOccurrence();
    }

    [[nodiscard]] const std::byte* findTypeDescriptorName(const std::byte* typeNameAddress) const noexcept
    {
        if (typeNameAddress)
            return HybridPatternFinder{dataRelRoSection.raw(), BinaryBytePattern{typeNameAddress}}.findNextOccurrence();
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
