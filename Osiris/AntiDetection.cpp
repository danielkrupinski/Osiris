#include "AntiDetection.h"
#include <Windows.h>

HMODULE GetSelfModuleHandle()
{
	MEMORY_BASIC_INFORMATION mbi;

	return ((::VirtualQuery(GetSelfModuleHandle, &mbi, sizeof(mbi)) != 0)
		? (HMODULE)mbi.AllocationBase : NULL);
}

void HideModule(void* pModule)
{
	void* pPEB = nullptr;

	_asm
	{
		push eax
		mov eax, fs: [0x30]
		mov pPEB, eax
		pop eax
	}

	void* pLDR = *((void**)((unsigned char*)pPEB + 0xc));
	void* pCurrent = *((void**)((unsigned char*)pLDR + 0x0c));
	void* pNext = pCurrent;

	do
	{
		void* pNextPoint = *((void**)((unsigned char*)pNext));
		void* pLastPoint = *((void**)((unsigned char*)pNext + 0x4));
		void* nBaseAddress = *((void**)((unsigned char*)pNext + 0x18));

		if (nBaseAddress == pModule)
		{
			*((void**)((unsigned char*)pLastPoint)) = pNextPoint;
			*((void**)((unsigned char*)pNextPoint + 0x4)) = pLastPoint;
			pCurrent = pNextPoint;
		}

		pNext = *((void**)pNext);
	} while (pCurrent != pNext);
}
AntiDetection::AntiDetection() {
	//clear up PE headers.
	HMODULE hModule = GetSelfModuleHandle();
	DWORD dwMemPro;
	VirtualProtect((void*)hModule, 0x1000, PAGE_EXECUTE_READWRITE, &dwMemPro);
	memset((void*)hModule, 0, 0x1000);
	VirtualProtect((void*)hModule, 0x1000, dwMemPro, &dwMemPro);
	OutputDebugStringA("CleanUp PEheader Success.");
	HideModule(hModule);
	OutputDebugStringA("Cutup PEB link success.");
}