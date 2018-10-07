#pragma once

#include "IHandleEntity.hpp"

#define NUM_ENT_ENTRY_BITS         (11 + 2)
#define NUM_ENT_ENTRIES            (1 << NUM_ENT_ENTRY_BITS)
#define INVALID_EHANDLE_INDEX       0xFFFFFFFF
#define NUM_SERIAL_NUM_BITS        16 // (32 - NUM_ENT_ENTRY_BITS)
#define NUM_SERIAL_NUM_SHIFT_BITS (32 - NUM_SERIAL_NUM_BITS)
#define ENT_ENTRY_MASK             (( 1 << NUM_SERIAL_NUM_BITS) - 1)

class IHandleEntity;

class CBaseHandle
{
public:
    CBaseHandle();
    CBaseHandle(const CBaseHandle &other);
    CBaseHandle(unsigned long value);
    CBaseHandle(int iEntry, int iSerialNumber);

    void Init(int iEntry, int iSerialNumber);
    void Term();

    // Even if this returns true, Get() still can return return a non-null value.
    // This just tells if the handle has been initted with any values.
    bool IsValid() const;

    int GetEntryIndex() const;
    int GetSerialNumber() const;

    int ToInt() const;
    bool operator !=(const CBaseHandle &other) const;
    bool operator ==(const CBaseHandle &other) const;
    bool operator ==(const IHandleEntity* pEnt) const;
    bool operator !=(const IHandleEntity* pEnt) const;
    bool operator <(const CBaseHandle &other) const;
    bool operator <(const IHandleEntity* pEnt) const;

    // Assign a value to the handle.
    const CBaseHandle& operator=(const IHandleEntity *pEntity);
    const CBaseHandle& Set(const IHandleEntity *pEntity);

    // Use this to dereference the handle.
    // Note: this is implemented in game code (ehandle.h)
    IHandleEntity* Get() const;

protected:
    // The low NUM_SERIAL_BITS hold the index. If this value is less than MAX_EDICTS, then the entity is networkable.
    // The high NUM_SERIAL_NUM_BITS bits are the serial number.
    unsigned long	m_Index;
};

inline CBaseHandle::CBaseHandle()
{
    m_Index = INVALID_EHANDLE_INDEX;
}

inline CBaseHandle::CBaseHandle(const CBaseHandle &other)
{
    m_Index = other.m_Index;
}

inline CBaseHandle::CBaseHandle(unsigned long value)
{
    m_Index = value;
}

inline CBaseHandle::CBaseHandle(int iEntry, int iSerialNumber)
{
    Init(iEntry, iSerialNumber);
}

inline void CBaseHandle::Init(int iEntry, int iSerialNumber)
{
    m_Index = iEntry | (iSerialNumber << NUM_ENT_ENTRY_BITS);
}

inline void CBaseHandle::Term()
{
    m_Index = INVALID_EHANDLE_INDEX;
}

inline bool CBaseHandle::IsValid() const
{
    return m_Index != INVALID_EHANDLE_INDEX;
}

inline int CBaseHandle::GetEntryIndex() const
{
    return m_Index & ENT_ENTRY_MASK;
}

inline int CBaseHandle::GetSerialNumber() const
{
    return m_Index >> NUM_ENT_ENTRY_BITS;
}

inline int CBaseHandle::ToInt() const
{
    return (int)m_Index;
}

inline bool CBaseHandle::operator !=(const CBaseHandle &other) const
{
    return m_Index != other.m_Index;
}

inline bool CBaseHandle::operator ==(const CBaseHandle &other) const
{
    return m_Index == other.m_Index;
}

inline bool CBaseHandle::operator ==(const IHandleEntity* pEnt) const
{
    return Get() == pEnt;
}

inline bool CBaseHandle::operator !=(const IHandleEntity* pEnt) const
{
    return Get() != pEnt;
}

inline bool CBaseHandle::operator <(const CBaseHandle &other) const
{
    return m_Index < other.m_Index;
}

inline bool CBaseHandle::operator <(const IHandleEntity *pEntity) const
{
    unsigned long otherIndex = (pEntity) ? pEntity->GetRefEHandle().m_Index : INVALID_EHANDLE_INDEX;
    return m_Index < otherIndex;
}

inline const CBaseHandle& CBaseHandle::operator=(const IHandleEntity *pEntity)
{
    return Set(pEntity);
}

inline const CBaseHandle& CBaseHandle::Set(const IHandleEntity *pEntity)
{
    if(pEntity) {
        *this = pEntity->GetRefEHandle();
    } else {
        m_Index = INVALID_EHANDLE_INDEX;
    }

    return *this;
}