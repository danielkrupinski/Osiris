#pragma once

/*
    https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/public/tier0/memalloc.h
    Structures aren't checked, if using, refer to your favorite disassembler.
    Needed for certain things when messing with the source engine (example: rebuilding anim stuff and allocating mem with
    win32 functions will cause an unexpected crash.)
*/

struct _CrtMemState;

#define MEMALLOC_VERSION 1

typedef size_t(*MemAllocFailHandler_t) (size_t);

//-----------------------------------------------------------------------------
// NOTE! This should never be called directly from leaf code
// Just use new,delete,malloc,free etc. They will call into this eventually
//-----------------------------------------------------------------------------
class MemAlloc
{
public:
    virtual ~MemAlloc();

    // Release versions
    virtual void* Alloc(size_t nSize) = 0;
    virtual void* Realloc(void* pMem, size_t nSize) = 0;
    virtual void Free(void* pMem) = 0;
    virtual void* Expand_NoLongerSupported(void* pMem, size_t nSize) = 0;

    // Debug versions
    //virtual void *Alloc(size_t nSize, const char *pFileName, int nLine) = 0;
    //virtual void *Realloc(void *pMem, size_t nSize, const char *pFileName, int nLine) = 0;
    //virtual void  Free(void *pMem, const char *pFileName, int nLine) = 0;
    //virtual void *Expand_NoLongerSupported(void *pMem, size_t nSize, const char *pFileName, int nLine) = 0;

    // Returns size of a particular allocation
    virtual size_t GetSize(void* pMem) = 0;

    // Force file + line information for an allocation
    virtual void PushAllocDbgInfo(const char* pFileName, int nLine) = 0;
    virtual void PopAllocDbgInfo() = 0;

    // FIXME: Remove when we have our own allocator
    // these methods of the Crt debug code is used in our codebase currently
    virtual long CrtSetBreakAlloc(long lNewBreakAlloc) = 0;
    virtual int CrtSetReportMode(int nReportType, int nReportMode) = 0;
    virtual int CrtIsValidHeapPointer(const void* pMem) = 0;
    virtual int CrtIsValidPointer(const void* pMem, unsigned int size, int access) = 0;
    virtual int CrtCheckMemory(void) = 0;
    virtual int CrtSetDbgFlag(int nNewFlag) = 0;
    virtual void CrtMemCheckpoint(_CrtMemState* pState) = 0;

    // FIXME: Make a better stats interface
    virtual void DumpStats() = 0;
    virtual void DumpStatsFileBase(char const* pchFileBase) = 0;

    // FIXME: Remove when we have our own allocator
    virtual void* CrtSetReportFile(int nRptType, void* hFile) = 0;
    virtual void* CrtSetReportHook(void* pfnNewHook) = 0;
    virtual int CrtDbgReport(int nRptType, const char* szFile,
        int nLine, const char* szModule, const char* pMsg) = 0;

    virtual int heapchk() = 0;

    virtual bool IsDebugHeap() = 0;

    virtual void GetActualDbgInfo(const char*& pFileName, int& nLine) = 0;
    virtual void RegisterAllocation(const char* pFileName, int nLine, int nLogicalSize, int nActualSize, unsigned nTime) = 0;
    virtual void RegisterDeallocation(const char* pFileName, int nLine, int nLogicalSize, int nActualSize, unsigned nTime) = 0;

    virtual int GetVersion() = 0;

    virtual void CompactHeap() = 0;

    // Function called when malloc fails or memory limits hit to attempt to free up memory (can come in any thread)
    virtual MemAllocFailHandler_t SetAllocFailHandler(MemAllocFailHandler_t pfnMemAllocFailHandler) = 0;

    virtual void DumpBlockStats(void*) = 0;

#if defined( _MEMTEST )
    virtual void SetStatsExtraInfo(const char* pMapName, const char* pComment) = 0;
#endif

    // Returns 0 if no failure, otherwise the size_t of the last requested chunk
    //  "I'm sure this is completely thread safe!" Brian Deen 7/19/2012.
    virtual size_t MemoryAllocFailed() = 0;

    // handles storing allocation info for coroutines
    virtual int GetDebugInfoSize() = 0;
    virtual void SaveDebugInfo(void* pvDebugInfo) = 0;
    virtual void RestoreDebugInfo(const void* pvDebugInfo) = 0;
    virtual void InitDebugInfo(void* pvDebugInfo, const char* pchRootFileName, int nLine) = 0;

    // Replacement for ::GlobalMemoryStatus which accounts for unused memory in our system
    virtual void GlobalMemoryStatus(size_t* pUsedMemory, size_t* pFreeMemory) = 0;
}; 