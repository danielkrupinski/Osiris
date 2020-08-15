#include "Pattern.h"

unsigned int Pattern::FindPattern(std::string moduleName, std::string pattern, bool relativeOffset) noexcept
{
	const char* pat = pattern.c_str();
	uintptr_t firstMatch = 0;
	uintptr_t rangeStart = (uintptr_t)GetModuleHandleA(moduleName.c_str());
	MODULEINFO miModInfo; GetModuleInformation(GetCurrentProcess(), (HMODULE)rangeStart, &miModInfo, sizeof(MODULEINFO));
	uintptr_t rangeEnd = rangeStart + miModInfo.SizeOfImage;

	for (uintptr_t pCur = rangeStart; pCur < rangeEnd; pCur++) {
		if (!*pat)
			break;

		if (*(PBYTE)pat == '\?' || *(BYTE*)pCur == getByte(pat)) {
			if (!firstMatch)
				firstMatch = pCur;

			if (!pat[2])
				break;

			if (*(PWORD)pat == '\?\?' || *(PBYTE)pat != '\?')
				pat += 3;
			else
				pat += 2;
		}
		else {
			pat = pattern.c_str();
			firstMatch = 0;
		}
	}

	if (relativeOffset)
		firstMatch = firstMatch + 0x5 + *(DWORD*)(firstMatch + 0x1);
	return firstMatch;
} 