#pragma once

#include <string_view>

#include <MemorySearch/HybridPatternFinder.h>
#include <Utils/MemorySection.h>

#include "RTTI/RttiCompleteObjectLocatorFinder.h"
#include "RTTI/RttiTypeDescriptorFinder.h"
#include "WindowsVmtFinderParams.h"

class WindowsVmtFinder {
public:
    explicit WindowsVmtFinder(const WindowsVmtFinderParams& params) noexcept
        : dataSection{params.dataSection}
        , vmtSection{params.rdataSection}
        , dllBase{params.dllBase}
    {
    }

    [[nodiscard]] const void* findVmt(std::string_view mangledTypeName) const noexcept
    {
        static constexpr auto kOffsetToVmt{8};

        const auto completeObjectLocatorPrecedingVmt{findCompleteObjectLocatorPrecedingVmt(mangledTypeName)};
        if (vmtSection.contains(reinterpret_cast<std::uintptr_t>(completeObjectLocatorPrecedingVmt), kOffsetToVmt))
            return completeObjectLocatorPrecedingVmt + kOffsetToVmt;
        return nullptr;
    }

private:
    [[nodiscard]] const std::byte* findCompleteObjectLocatorPrecedingVmt(std::string_view mangledTypeName) const noexcept
    {
        return findCompleteObjectLocatorReference(findCompleteObjectLocator(findTypeDescriptor(mangledTypeName)));
    }

    [[nodiscard]] const RttiTypeDescriptor* findTypeDescriptor(std::string_view mangledTypeName) const noexcept
    {
        return RttiTypeDescriptorFinder{dataSection}.findTypeDescriptor(mangledTypeName);
    }

    [[nodiscard]] const RttiCompleteObjectLocator* findCompleteObjectLocator(const RttiTypeDescriptor* typeDescriptor) const noexcept
    {
        if (typeDescriptor)
            return RttiCompleteObjectLocatorFinder{vmtSection, ToRvaConverter{reinterpret_cast<std::uintptr_t>(dllBase)}}.findCompleteObjectLocator(typeDescriptor);
        return nullptr;
    }

    [[nodiscard]] const std::byte* findCompleteObjectLocatorReference(const RttiCompleteObjectLocator* completeObjectLocator) const noexcept
    {
        if (completeObjectLocator)
            return HybridPatternFinder{vmtSection.raw(), BytePattern::ofObject(completeObjectLocator)}.findNextOccurrence();
        return nullptr;
    }

    MemorySection dataSection;
    MemorySection vmtSection;
    const void* dllBase;
};
