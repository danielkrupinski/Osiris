#include "AntiDetection.h"

#ifdef _WIN32

#include <memory>

bool AntiDetection::RemoveHeader() noexcept {
    if (peHeader.data)
        return false;

    // Get DOS Header
    const auto pDosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(hModule);
    // Get NT Header
    const auto pNTHeader = reinterpret_cast<PIMAGE_NT_HEADERS>(reinterpret_cast<PBYTE>(pDosHeader) + pDosHeader->e_lfanew);

    // Check if NT Header is correct
    if (pNTHeader->Signature != IMAGE_NT_SIGNATURE)
        return false;

    // Return value
    bool result = false;

    // Check if Header exists and get PE Header size
    if ((peHeader.size = pNTHeader->FileHeader.SizeOfOptionalHeader)) {
        // Store VirtualProtect flags
        DWORD dwMemProtect;
        // Disable memory protect
        if (VirtualProtect(hModule, peHeader.size, PAGE_EXECUTE_READWRITE, &dwMemProtect)) {
            // Allocate memory to PE Header backup
            peHeader.data = std::make_unique<std::byte[]>(peHeader.size);
            // Backup PE Header
            if (std::memcpy(peHeader.data.get(), hModule, peHeader.size) == hModule)
                // Erease PE Header
                if (SecureZeroMemory(hModule, peHeader.size))
                    result = true;
            // Restore memory protect
            if (!VirtualProtect(hModule, peHeader.size, dwMemProtect, &dwMemProtect))
                result = false;
            // Release and delete peHeader if something wrong
            if (!result) {
                peHeader.data.reset();
                peHeader.size = 0;
            }
        }
    }
    return result;
}

bool AntiDetection::RestoreHeader() noexcept {
    if (!peHeader.data)
        return false;

    // Return value
    bool result = false;

    // Store VirtualProtect flags
    DWORD dwMemProtect = NULL;
    // Disable memory protect
    if (VirtualProtect(hModule, peHeader.size, PAGE_EXECUTE_READWRITE, &dwMemProtect)) {
        // Restore PE Header
        if (std::memcpy(hModule, peHeader.data.get(), peHeader.size) == hModule)
            result = true;
        // Restore memory protect
        if (!VirtualProtect(hModule, peHeader.size, dwMemProtect, &dwMemProtect))
            result = false;
        // Release and delete peHeader
        peHeader.data.reset();
        peHeader.size = 0;
    }

    return result;
}

bool AntiDetection::RemoveModule() noexcept {
    // Check if module already unlinked
    if (UnlinkedModule != nullptr)
        return false;

    // Get ProcessEnvironmentBlock
    const auto pPEB = reinterpret_cast<PPEB>(__readfsdword(FIELD_OFFSET(TEB, ProcessEnvironmentBlock)));

    // Cast PEB Loader Data as custom (PPEB_LDR_DATA_FULL) struct
    auto* pLDR = reinterpret_cast<PPEB_LDR_DATA_FULL>(pPEB->Ldr);
    PLIST_ENTRY CurrentEntry = pLDR->InLoadOrderModuleList.Flink;
    // Find entry with needed hModule
    while (CurrentEntry != &pLDR->InLoadOrderModuleList && CurrentEntry != nullptr) {
        if (const auto Current = CONTAINING_RECORD(CurrentEntry, LDR_DATA_TABLE_ENTRY_FULL, InLoadOrderLinks);
            Current->DllBase == hModule)
        {
            // Backup current module state
            const UNLINKED_MODULE CurrentModule{
                hModule,
                Current->InLoadOrderLinks.Blink->Flink,
                Current->InMemoryOrderLinks.Blink->Flink,
                Current->InInitializationOrderLinks.Blink->Flink,
                Current
            };
            // Push backup to vector
            UnlinkedModule = std::make_unique<UNLINKED_MODULE>(CurrentModule);

            // Unlink all
            UnlinkLink(&Current->InLoadOrderLinks);
            UnlinkLink(&Current->InMemoryOrderLinks);
            UnlinkLink(&Current->InInitializationOrderLinks);

            return true;
        }

        CurrentEntry = CurrentEntry->Flink;
    }

    return false;
}

bool AntiDetection::RestoreModule() noexcept {
    // Check if module already linked
    if (UnlinkedModule == nullptr)
        return false;

    // Relink all from backup
    RelinkLink(UnlinkedModule->RealInLoadOrderLinks, &UnlinkedModule->LdrDataTableEntry->InLoadOrderLinks);
    RelinkLink(UnlinkedModule->RealInMemoryOrderLinks, &UnlinkedModule->LdrDataTableEntry->InMemoryOrderLinks);
    RelinkLink(UnlinkedModule->RealInInitializationOrderLinks, &UnlinkedModule->LdrDataTableEntry->InInitializationOrderLinks);
    // Delete backup
    UnlinkedModule.reset();

    return true;
}

#endif
