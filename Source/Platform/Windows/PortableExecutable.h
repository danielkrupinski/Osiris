#pragma once

#include <cassert>
#include <cstddef>
#include <cstring>
#include <span>

#include <Windows.h>

class PortableExecutable {
public:
    explicit PortableExecutable(const std::byte* base)
        : base{ base }
    {
        assert(base);
    }

    [[nodiscard]] std::span<const std::byte> getCodeSection() const noexcept
    {
        for (const auto& section : getSectionHeaders()) {
            if ((section.Characteristics & IMAGE_SCN_MEM_EXECUTE) != 0 && std::memcmp(section.Name, ".text", 5) == 0)
                return { base + section.VirtualAddress, section.Misc.VirtualSize };
        }
        return {};
    }

    [[nodiscard]] const void* getExport(const char* name) const noexcept
    {
        const auto exportDirectory = getExportDirectory();
        if (!exportDirectory)
            return nullptr;

        for (DWORD i = 0; i < exportDirectory->NumberOfNames; ++i) {
            const auto exportName = reinterpret_cast<const char*>(base + reinterpret_cast<const DWORD*>(base + exportDirectory->AddressOfNames)[i]);
            if (std::strcmp(exportName, name) == 0) {
                const auto nameOrdinals = reinterpret_cast<const WORD*>(base + exportDirectory->AddressOfNameOrdinals);
                const auto functions = reinterpret_cast<const DWORD*>(base + exportDirectory->AddressOfFunctions);
                return base + functions[nameOrdinals[i]];
            }
        }

        return nullptr;
    }

private:
    [[nodiscard]] std::span<const IMAGE_SECTION_HEADER> getSectionHeaders() const noexcept
    {
        if (const auto ntHeaders = getNtHeaders())
            return { reinterpret_cast<const IMAGE_SECTION_HEADER*>(reinterpret_cast<const std::byte*>(&ntHeaders->OptionalHeader) + ntHeaders->FileHeader.SizeOfOptionalHeader),
                     ntHeaders->FileHeader.NumberOfSections };
        return {};
    }

    [[nodiscard]] const IMAGE_EXPORT_DIRECTORY* getExportDirectory() const noexcept
    {
        if (const auto exportDirectory = getDataDirectory(IMAGE_DIRECTORY_ENTRY_EXPORT); exportDirectory && exportDirectory->Size >= sizeof(IMAGE_EXPORT_DIRECTORY))
            return reinterpret_cast<const IMAGE_EXPORT_DIRECTORY*>(base + exportDirectory->VirtualAddress);
        return nullptr;
    }

    [[nodiscard]] const IMAGE_DATA_DIRECTORY* getDataDirectory(std::uint8_t entry) const noexcept
    {
        if (const auto optionalHeader = getOptionalHeader(); optionalHeader && optionalHeader->NumberOfRvaAndSizes > entry)
            return &optionalHeader->DataDirectory[entry];
        return nullptr;
    }

    [[nodiscard]] const IMAGE_OPTIONAL_HEADER* getOptionalHeader() const noexcept
    {
        if (const auto ntHeaders = getNtHeaders(); ntHeaders && ntHeaders->OptionalHeader.Magic == IMAGE_NT_OPTIONAL_HDR_MAGIC)
            return &ntHeaders->OptionalHeader;
        return nullptr;
    }

    [[nodiscard]] const IMAGE_NT_HEADERS* getNtHeaders() const noexcept
    {
        if (const auto dosHeader = getDosHeader())
            return getNtHeaders(*dosHeader);
        return nullptr;
    }

    [[nodiscard]] const IMAGE_NT_HEADERS* getNtHeaders(const IMAGE_DOS_HEADER& dosHeader) const noexcept
    {
        constexpr auto peSignature = 0x4550;
        if (const auto ntHeaders = reinterpret_cast<const IMAGE_NT_HEADERS*>(base + dosHeader.e_lfanew); ntHeaders->Signature == peSignature)
            return ntHeaders;
        return nullptr;
    }

    [[nodiscard]] const IMAGE_DOS_HEADER* getDosHeader() const noexcept
    {
        if (const auto dosHeader = reinterpret_cast<const IMAGE_DOS_HEADER*>(base); dosHeader->e_magic == IMAGE_DOS_SIGNATURE)
            return dosHeader;
        return nullptr;
    }

    const std::byte* base;
};
