#pragma once

#include <cstdint>
#include <cstring>

#include "UtlMemory.hpp"

class CUtlBinaryBlock
{
public:
    CUtlBinaryBlock(int growSize = 0, int initSize = 0);

    // NOTE: nInitialLength indicates how much of the buffer starts full
    CUtlBinaryBlock(void* pMemory, int nSizeInBytes, int nInitialLength);
    CUtlBinaryBlock(const void* pMemory, int nSizeInBytes);
    CUtlBinaryBlock(const CUtlBinaryBlock& src);

    void        Get(void *pValue, int nMaxLen) const;
    void        Set(const void *pValue, int nLen);
    const void  *Get() const;
    void        *Get();

    unsigned char& operator[](int i);
    const unsigned char& operator[](int i) const;

    int         Length() const;
    void        SetLength(int nLength);    // Undefined memory will result
    bool        IsEmpty() const;
    void        Clear();
    void        Purge();

    bool        IsReadOnly() const;

    CUtlBinaryBlock &operator=(const CUtlBinaryBlock &src);

    // Test for equality
    bool operator==(const CUtlBinaryBlock &src) const;

private:
    CUtlMemory<unsigned char> m_Memory;
    int m_nActualLength;
};


//-----------------------------------------------------------------------------
// class inlines
//-----------------------------------------------------------------------------
inline const void *CUtlBinaryBlock::Get() const
{
    return m_Memory.Base();
}

inline void *CUtlBinaryBlock::Get()
{
    return m_Memory.Base();
}

inline int CUtlBinaryBlock::Length() const
{
    return m_nActualLength;
}

inline unsigned char& CUtlBinaryBlock::operator[](int i)
{
    return m_Memory[i];
}

inline const unsigned char& CUtlBinaryBlock::operator[](int i) const
{
    return m_Memory[i];
}

inline bool CUtlBinaryBlock::IsReadOnly() const
{
    return m_Memory.IsReadOnly();
}

inline bool CUtlBinaryBlock::IsEmpty() const
{
    return Length() == 0;
}

inline void CUtlBinaryBlock::Clear()
{
    SetLength(0);
}

inline void CUtlBinaryBlock::Purge()
{
    SetLength(0);
    m_Memory.Purge();
}

//-----------------------------------------------------------------------------
// Simple string class. 
// NOTE: This is *not* optimal! Use in tools, but not runtime code
//-----------------------------------------------------------------------------
class CUtlString
{
public:
    CUtlString();
    CUtlString(const char *pString);
    CUtlString(const CUtlString& string);

    // Attaches the string to external memory. Useful for avoiding a copy
    CUtlString(void* pMemory, int nSizeInBytes, int nInitialLength);
    CUtlString(const void* pMemory, int nSizeInBytes);

    const char    *Get() const;
    void        Set(const char *pValue);

    // Set directly and don't look for a null terminator in pValue.
    void        SetDirect(const char *pValue, int nChars);

    // Converts to c-strings
    operator const char*() const;

    // for compatibility switching items from UtlSymbol
    const char  *String() const { return Get(); }

    // Returns strlen
    int            Length() const;
    bool        IsEmpty() const;

    // Sets the length (used to serialize into the buffer )
    // Note: If nLen != 0, then this adds an extra uint8_t for a null-terminator.    
    void        SetLength(int nLen);
    char        *Get();
    void        Clear();
    void        Purge();

    // Strips the trailing slash
    void        StripTrailingSlash();

    CUtlString &operator=(const CUtlString &src);
    CUtlString &operator=(const char *src);

    // Test for equality
    bool operator==(const CUtlString &src) const;
    bool operator==(const char *src) const;
    bool operator!=(const CUtlString &src) const { return !operator==(src); }
    bool operator!=(const char *src) const { return !operator==(src); }

    CUtlString &operator+=(const CUtlString &rhs);
    CUtlString &operator+=(const char *rhs);
    CUtlString &operator+=(char c);
    CUtlString &operator+=(int rhs);
    CUtlString &operator+=(double rhs);

    CUtlString operator+(const char *pOther);
    CUtlString operator+(int rhs);

    int Format(const char *pFormat, ...);

    // Take a piece out of the string.
    // If you only specify nStart, it'll go from nStart to the end.
    // You can use negative numbers and it'll wrap around to the start.
    CUtlString Slice(int32_t nStart = 0, int32_t nEnd = INT32_MAX);

    // Grab a substring starting from the left or the right side.
    CUtlString Left(int32_t nChars);
    CUtlString Right(int32_t nChars);

    // Replace all instances of one character with another.
    CUtlString Replace(char cFrom, char cTo);

    // Calls right through to V_MakeAbsolutePath.
    CUtlString AbsPath(const char *pStartingDir = NULL);

    // Gets the filename (everything except the path.. c:\a\b\c\somefile.txt -> somefile.txt).
    CUtlString UnqualifiedFilename();

    // Strips off one directory. Uses V_StripLastDir but strips the last slash also!
    CUtlString DirName();

    // Works like V_ComposeFileName.
    static CUtlString PathJoin(const char *pStr1, const char *pStr2);

    // These can be used for utlvector sorts.
    static int __cdecl SortCaseInsensitive(const CUtlString *pString1, const CUtlString *pString2);
    static int __cdecl SortCaseSensitive(const CUtlString *pString1, const CUtlString *pString2);

private:
    CUtlBinaryBlock m_Storage;
};


//-----------------------------------------------------------------------------
// Inline methods
//-----------------------------------------------------------------------------
inline bool CUtlString::IsEmpty() const
{
    return Length() == 0;
}

inline int __cdecl CUtlString::SortCaseInsensitive(const CUtlString *pString1, const CUtlString *pString2)
{
    return _stricmp(pString1->String(), pString2->String());
}

inline int __cdecl CUtlString::SortCaseSensitive(const CUtlString *pString1, const CUtlString *pString2)
{
    return strcmp(pString1->String(), pString2->String());
}
