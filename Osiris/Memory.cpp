#include <Psapi.h>
#include <Windows.h>

#include "Memory.h"

std::uintptr_t Memory::findPattern(std::string module, std::string pattern)
{
    MODULEINFO moduleInfo{ };

    if (!GetModuleInformation(GetCurrentProcess(), GetModuleHandleA(module.c_str()), &moduleInfo, sizeof(moduleInfo)))
        return 0;

    const uintptr_t start_address = reinterpret_cast<uintptr_t>(moduleInfo.lpBaseOfDll);
    const uintptr_t end_address = start_address + moduleInfo.SizeOfImage;
    const char* scanPattern = pattern.c_str();

    uintptr_t first_match = 0;

    for (uintptr_t position = start_address; position < end_address; position++) {
        if (!*scanPattern)
            return first_match;

        const uint8_t pattern_current = *reinterpret_cast<const uint8_t*>(scanPattern);
        const uint8_t memory_current = *reinterpret_cast<const uint8_t*>(position);

        if (pattern_current == '\?' || memory_current == ((((scanPattern[0] & (~0x20)) >= 'A' && (scanPattern[0] & (~0x20)) <= 'F') ? ((scanPattern[0] & (~0x20)) - 'A' + 0xA) : ((scanPattern[0] >= '0' && scanPattern[0] <= '9') ? scanPattern[0] - '0' : 0)) << 4 | (((scanPattern[1] & (~0x20)) >= 'A' && (scanPattern[1] & (~0x20)) <= 'F') ? ((scanPattern[1] & (~0x20)) - 'A' + 0xA) : ((scanPattern[1] >= '0' && scanPattern[1] <= '9') ? scanPattern[1] - '0' : 0)))) {
            if (!first_match)
                first_match = position;

            if (!scanPattern[2])
                return first_match;

            scanPattern += pattern_current != '\?' ? 3 : 2;
        }
        else {
            scanPattern = pattern.c_str();
            first_match = 0;
        }
    }
}
