#pragma once

#include <cassert>
#include <cstddef>
#include <cstring>
#include <span>

#include <Windows.h>

#include <Utils/MemorySection.h>
#include <Utils/SafeAddress.h>

class PortableExecutable {
public:
    explicit PortableExecutable(const std::byte* base)
        : base{ base }
    {
        assert(base);
    }

    [[nodiscard]] MemorySection getCodeSection() const noexcept
    {
        for (const auto& section : getSectionHeaders()) {
            if ((section.Characteristics & IMAGE_SCN_MEM_EXECUTE) != 0 && std::memcmp(section.Name, ".text", 5) == 0)
                return MemorySection{ std::span{ base + section.VirtualAddress, section.Misc.VirtualSize } };
        }
        return {};
    }

    [[nodiscard]] MemorySection getVmtSection() const noexcept
    {
        for (const auto& section : getSectionHeaders()) {
            if ((section.Characteristics & IMAGE_SCN_MEM_READ) != 0 && std::memcmp(section.Name, ".rdata", 6) == 0)
                return MemorySection{ std::span{ base + section.VirtualAddress, section.Misc.VirtualSize } };
        }
        return {};
    }

    [[nodiscard]] SafeAddress getExport(const char* name) const noexcept
    {
        const auto exportDataDirectory = getExportDataDirectory();
        if (!exportDataDirectory)
            return SafeAddress{ 0 };

        const auto exportDirectory = reinterpret_cast<const IMAGE_EXPORT_DIRECTORY*>(base + exportDataDirectory->VirtualAddress);

        for (DWORD i = 0; i < exportDirectory->NumberOfNames; ++i) {
            const auto exportName = reinterpret_cast<const char*>(base + reinterpret_cast<const DWORD*>(base + exportDirectory->AddressOfNames)[i]);
            if (std::strcmp(exportName, name) == 0) {
                const auto nameOrdinals = reinterpret_cast<const WORD*>(base + exportDirectory->AddressOfNameOrdinals);
                const auto functions = reinterpret_cast<const DWORD*>(base + exportDirectory->AddressOfFunctions);
                const auto functionRva = functions[nameOrdinals[i]];
                if (isForwardedExport(functionRva, *exportDataDirectory)) {
                    assert(false && "Forwarded exports are not supported yet!");
                    return SafeAddress{ 0 };
                }
                return SafeAddress{ base + functionRva };
            }
        }

        return SafeAddress{ 0 };
    }

private:
    [[nodiscard]] static bool isForwardedExport(DWORD functionRva, const IMAGE_DATA_DIRECTORY& exportDataDirectory) noexcept
    {
        return functionRva >= exportDataDirectory.VirtualAddress && functionRva - exportDataDirectory.VirtualAddress < exportDataDirectory.Size;
    }

    [[nodiscard]] std::span<const IMAGE_SECTION_HEADER> getSectionHeaders() const noexcept
    {
        if (const auto ntHeaders = getNtHeaders())
            return { reinterpret_cast<const IMAGE_SECTION_HEADER*>(reinterpret_cast<const std::byte*>(&ntHeaders->OptionalHeader) + ntHeaders->FileHeader.SizeOfOptionalHeader),
                     ntHeaders->FileHeader.NumberOfSections };
        return {};
    }

    [[nodiscard]] const IMAGE_DATA_DIRECTORY* getExportDataDirectory() const noexcept
    {
        return getDataDirectory(IMAGE_DIRECTORY_ENTRY_EXPORT);
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
