#pragma once

#include <cstring>
#include "UtlMemory.hpp"

template< class T, class A = CUtlMemory<T> >
class CUtlVector
{
    typedef A CAllocator;
public:
    typedef T ElemType_t;

    // constructor, destructor
    CUtlVector(int growSize = 0, int initSize = 0);
    CUtlVector(T* pMemory, int allocationCount, int numElements = 0);
    ~CUtlVector();

    // Copy the array.
    CUtlVector<T, A>& operator=(const CUtlVector<T, A> &other);

    // element access
    T& operator[](int i);
    const T& operator[](int i) const;
    T& Element(int i);
    const T& Element(int i) const;
    T& Head();
    const T& Head() const;
    T& Tail();
    const T& Tail() const;

    // Gets the base address (can change when adding elements!)
    T* Base() { return m_Memory.Base(); }
    const T* Base() const { return m_Memory.Base(); }
    // Returns the number of elements in the vector
    int Count() const;
    // Is element index valid?
    bool IsValidIndex(int i) const;
    static int InvalidIndex();
    // Adds an element, uses default constructor
    int AddToHead();
    int AddToTail();
    int InsertBefore(int elem);
    int InsertAfter(int elem);
    // Adds an element, uses copy constructor
    int AddToHead(const T& src);
    int AddToTail(const T& src);
    int InsertBefore(int elem, const T& src);
    int InsertAfter(int elem, const T& src);
    // Adds multiple elements, uses default constructor
    int AddMultipleToHead(int num);
    int AddMultipleToTail(int num);
    int AddMultipleToTail(int num, const T *pToCopy);
    int InsertMultipleBefore(int elem, int num);
    int InsertMultipleBefore(int elem, int num, const T *pToCopy);
    int InsertMultipleAfter(int elem, int num);
    // Calls RemoveAll() then AddMultipleToTail.
    void SetSize(int size);
    void SetCount(int count);
    void SetCountNonDestructively(int count); //sets count by adding or removing elements to tail TODO: This should probably be the default behavior for SetCount
    void CopyArray(const T *pArray, int size); //Calls SetSize and copies each element.
                                               // Fast swap
    void Swap(CUtlVector< T, A > &vec);
    // Add the specified array to the tail.
    int AddVectorToTail(CUtlVector<T, A> const &src);
    // Finds an element (element needs operator== defined)
    int GetOffset(const T& src) const;
    void FillWithValue(const T& src);
    bool HasElement(const T& src) const;
    // Makes sure we have enough memory allocated to store a requested # of elements
    void EnsureCapacity(int num);
    // Makes sure we have at least this many elements
    void EnsureCount(int num);
    // Element removal
    void FastRemove(int elem);    // doesn't preserve order
    void Remove(int elem);        // preserves order, shifts elements
    bool FindAndRemove(const T& src);    // removes first occurrence of src, preserves order, shifts elements
    bool FindAndFastRemove(const T& src);    // removes first occurrence of src, doesn't preserve order
    void RemoveMultiple(int elem, int num);    // preserves order, shifts elements
    void RemoveMultipleFromHead(int num); // removes num elements from tail
    void RemoveMultipleFromTail(int num); // removes num elements from tail
    void RemoveAll();                // doesn't deallocate memory
    void Purge(); // Memory deallocation
                  // Purges the list and calls delete on each element in it.
    void PurgeAndDeleteElements();
    // Compacts the vector to the number of elements actually in use 
    void Compact();
    // Set the size by which it grows when it needs to allocate more memory.
    void SetGrowSize(int size) { m_Memory.SetGrowSize(size); }
    int NumAllocated() const;    // Only use this if you really know what you're doing!
    void Sort(int(__cdecl *pfnCompare)(const T *, const T *));

protected:
    // Can't copy this unless we explicitly do it!
    CUtlVector(CUtlVector const& vec) { assert(0); }

    // Grows the vector
    void GrowVector(int num = 1);

    // Shifts elements....
    void ShiftElementsRight(int elem, int num = 1);
    void ShiftElementsLeft(int elem, int num = 1);

public:
    CAllocator m_Memory;
    int m_Size;

    // For easier access to the elements through the debugger
    // it's in release builds so this can be used in libraries correctly
    T *m_pElements;

    inline void ResetDbgInfo()
    {
        m_pElements = Base();
    }
};


//-----------------------------------------------------------------------------
// constructor, destructor
//-----------------------------------------------------------------------------
template< typename T, class A >
inline CUtlVector<T, A>::CUtlVector(int growSize, int initSize) :
    m_Memory(growSize, initSize), m_Size(0)
{
    ResetDbgInfo();
}

template< typename T, class A >
inline CUtlVector<T, A>::CUtlVector(T* pMemory, int allocationCount, int numElements) :
    m_Memory(pMemory, allocationCount), m_Size(numElements)
{
    ResetDbgInfo();
}

template< typename T, class A >
inline CUtlVector<T, A>::~CUtlVector()
{
    Purge();
}

template< typename T, class A >
inline CUtlVector<T, A>& CUtlVector<T, A>::operator=(const CUtlVector<T, A> &other)
{
    int nCount = other.Count();
    SetSize(nCount);
    for(int i = 0; i < nCount; i++) {
        (*this)[i] = other[i];
    }
    return *this;
}


//-----------------------------------------------------------------------------
// element access
//-----------------------------------------------------------------------------
template< typename T, class A >
inline T& CUtlVector<T, A>::operator[](int i)
{
    assert(i < m_Size);
    return m_Memory[i];
}

template< typename T, class A >
inline const T& CUtlVector<T, A>::operator[](int i) const
{
    assert(i < m_Size);
    return m_Memory[i];
}

template< typename T, class A >
inline T& CUtlVector<T, A>::Element(int i)
{
    assert(i < m_Size);
    return m_Memory[i];
}

template< typename T, class A >
inline const T& CUtlVector<T, A>::Element(int i) const
{
    assert(i < m_Size);
    return m_Memory[i];
}

template< typename T, class A >
inline T& CUtlVector<T, A>::Head()
{
    assert(m_Size > 0);
    return m_Memory[0];
}

template< typename T, class A >
inline const T& CUtlVector<T, A>::Head() const
{
    assert(m_Size > 0);
    return m_Memory[0];
}

template< typename T, class A >
inline T& CUtlVector<T, A>::Tail()
{
    assert(m_Size > 0);
    return m_Memory[m_Size - 1];
}

template< typename T, class A >
inline const T& CUtlVector<T, A>::Tail() const
{
    assert(m_Size > 0);
    return m_Memory[m_Size - 1];
}


//-----------------------------------------------------------------------------
// Count
//-----------------------------------------------------------------------------
template< typename T, class A >
inline int CUtlVector<T, A>::Count() const
{
    return m_Size;
}


//-----------------------------------------------------------------------------
// Is element index valid?
//-----------------------------------------------------------------------------
template< typename T, class A >
inline bool CUtlVector<T, A>::IsValidIndex(int i) const
{
    return (i >= 0) && (i < m_Size);
}


//-----------------------------------------------------------------------------
// Returns in invalid index
//-----------------------------------------------------------------------------
template< typename T, class A >
inline int CUtlVector<T, A>::InvalidIndex()
{
    return -1;
}


//-----------------------------------------------------------------------------
// Grows the vector
//-----------------------------------------------------------------------------
template< typename T, class A >
void CUtlVector<T, A>::GrowVector(int num)
{
    if(m_Size + num > m_Memory.NumAllocated()) {
        m_Memory.Grow(m_Size + num - m_Memory.NumAllocated());
    }

    m_Size += num;
    ResetDbgInfo();
}


//-----------------------------------------------------------------------------
// Sorts the vector
//-----------------------------------------------------------------------------
template< typename T, class A >
void CUtlVector<T, A>::Sort(int(__cdecl *pfnCompare)(const T *, const T *))
{
    typedef int(__cdecl *QSortCompareFunc_t)(const void *, const void *);
    if(Count() <= 1)
        return;

    if(Base()) {
        qsort(Base(), Count(), sizeof(T), (QSortCompareFunc_t)(pfnCompare));
    } else {
        assert(0);
        // this path is untested
        // if you want to sort vectors that use a non-sequential memory allocator,
        // you'll probably want to patch in a quicksort algorithm here
        // I just threw in this bubble sort to have something just in case...

        for(int i = m_Size - 1; i >= 0; --i) {
            for(int j = 1; j <= i; ++j) {
                if(pfnCompare(&Element(j - 1), &Element(j)) < 0) {
                    V_swap(Element(j - 1), Element(j));
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Makes sure we have enough memory allocated to store a requested # of elements
//-----------------------------------------------------------------------------
template< typename T, class A >
void CUtlVector<T, A>::EnsureCapacity(int num)
{
    MEM_ALLOC_CREDIT_CLASS();
    m_Memory.EnsureCapacity(num);
    ResetDbgInfo();
}


//-----------------------------------------------------------------------------
// Makes sure we have at least this many elements
//-----------------------------------------------------------------------------
template< typename T, class A >
void CUtlVector<T, A>::EnsureCount(int num)
{
    if(Count() < num) {
        AddMultipleToTail(num - Count());
    }
}


//-----------------------------------------------------------------------------
// Shifts elements
//-----------------------------------------------------------------------------
template< typename T, class A >
void CUtlVector<T, A>::ShiftElementsRight(int elem, int num)
{
    assert(IsValidIndex(elem) || (m_Size == 0) || (num == 0));
    int numToMove = m_Size - elem - num;
    if((numToMove > 0) && (num > 0))
        memmove(&Element(elem + num), &Element(elem), numToMove * sizeof(T));
}

template< typename T, class A >
void CUtlVector<T, A>::ShiftElementsLeft(int elem, int num)
{
    assert(IsValidIndex(elem) || (m_Size == 0) || (num == 0));
    int numToMove = m_Size - elem - num;
    if((numToMove > 0) && (num > 0)) {
        memmove(&Element(elem), &Element(elem + num), numToMove * sizeof(T));

#ifdef _DEBUG
        memset(&Element(m_Size - num), 0xDD, num * sizeof(T));
#endif
    }
}


//-----------------------------------------------------------------------------
// Adds an element, uses default constructor
//-----------------------------------------------------------------------------
template< typename T, class A >
inline int CUtlVector<T, A>::AddToHead()
{
    return InsertBefore(0);
}

template< typename T, class A >
inline int CUtlVector<T, A>::AddToTail()
{
    return InsertBefore(m_Size);
}

template< typename T, class A >
inline int CUtlVector<T, A>::InsertAfter(int elem)
{
    return InsertBefore(elem + 1);
}

template< typename T, class A >
int CUtlVector<T, A>::InsertBefore(int elem)
{
    // Can insert at the end
    assert((elem == Count()) || IsValidIndex(elem));

    GrowVector();
    ShiftElementsRight(elem);
    Construct(&Element(elem));
    return elem;
}


//-----------------------------------------------------------------------------
// Adds an element, uses copy constructor
//-----------------------------------------------------------------------------
template< typename T, class A >
inline int CUtlVector<T, A>::AddToHead(const T& src)
{
    // Can't insert something that's in the list... reallocation may hose us
    assert((Base() == NULL) || (&src < Base()) || (&src >= (Base() + Count())));
    return InsertBefore(0, src);
}

template< typename T, class A >
inline int CUtlVector<T, A>::AddToTail(const T& src)
{
    // Can't insert something that's in the list... reallocation may hose us
    assert((Base() == NULL) || (&src < Base()) || (&src >= (Base() + Count())));
    return InsertBefore(m_Size, src);
}

template< typename T, class A >
inline int CUtlVector<T, A>::InsertAfter(int elem, const T& src)
{
    // Can't insert something that's in the list... reallocation may hose us
    assert((Base() == NULL) || (&src < Base()) || (&src >= (Base() + Count())));
    return InsertBefore(elem + 1, src);
}

template< typename T, class A >
int CUtlVector<T, A>::InsertBefore(int elem, const T& src)
{
    // Can't insert something that's in the list... reallocation may hose us
    assert((Base() == NULL) || (&src < Base()) || (&src >= (Base() + Count())));

    // Can insert at the end
    assert((elem == Count()) || IsValidIndex(elem));

    GrowVector();
    ShiftElementsRight(elem);
    CopyConstruct(&Element(elem), src);
    return elem;
}


//-----------------------------------------------------------------------------
// Adds multiple elements, uses default constructor
//-----------------------------------------------------------------------------
template< typename T, class A >
inline int CUtlVector<T, A>::AddMultipleToHead(int num)
{
    return InsertMultipleBefore(0, num);
}

template< typename T, class A >
inline int CUtlVector<T, A>::AddMultipleToTail(int num)
{
    return InsertMultipleBefore(m_Size, num);
}

template< typename T, class A >
inline int CUtlVector<T, A>::AddMultipleToTail(int num, const T *pToCopy)
{
    // Can't insert something that's in the list... reallocation may hose us
    assert((Base() == NULL) || !pToCopy || (pToCopy + num <= Base()) || (pToCopy >= (Base() + Count())));

    return InsertMultipleBefore(m_Size, num, pToCopy);
}

template< typename T, class A >
int CUtlVector<T, A>::InsertMultipleAfter(int elem, int num)
{
    return InsertMultipleBefore(elem + 1, num);
}


template< typename T, class A >
void CUtlVector<T, A>::SetCount(int count)
{
    RemoveAll();
    AddMultipleToTail(count);
}

template< typename T, class A >
inline void CUtlVector<T, A>::SetSize(int size)
{
    SetCount(size);
}

template< typename T, class A >
void CUtlVector<T, A>::SetCountNonDestructively(int count)
{
    int delta = count - m_Size;
    if(delta > 0) AddMultipleToTail(delta);
    else if(delta < 0) RemoveMultipleFromTail(-delta);
}

template< typename T, class A >
void CUtlVector<T, A>::CopyArray(const T *pArray, int size)
{
    // Can't insert something that's in the list... reallocation may hose us
    assert((Base() == NULL) || !pArray || (Base() >= (pArray + size)) || (pArray >= (Base() + Count())));

    SetSize(size);
    for(int i = 0; i < size; i++) {
        (*this)[i] = pArray[i];
    }
}

template< typename T, class A >
void CUtlVector<T, A>::Swap(CUtlVector< T, A > &vec)
{
    m_Memory.Swap(vec.m_Memory);
    V_swap(m_Size, vec.m_Size);
#ifndef _X360
    V_swap(m_pElements, vec.m_pElements);
#endif
}

template< typename T, class A >
int CUtlVector<T, A>::AddVectorToTail(CUtlVector const &src)
{
    assert(&src != this);

    int base = Count();

    // Make space.
    int nSrcCount = src.Count();
    EnsureCapacity(base + nSrcCount);

    // Copy the elements.	
    m_Size += nSrcCount;
    for(int i = 0; i < nSrcCount; i++) {
        CopyConstruct(&Element(base + i), src[i]);
    }
    return base;
}

template< typename T, class A >
inline int CUtlVector<T, A>::InsertMultipleBefore(int elem, int num)
{
    if(num == 0)
        return elem;

    // Can insert at the end
    assert((elem == Count()) || IsValidIndex(elem));

    GrowVector(num);
    ShiftElementsRight(elem, num);

    // Invoke default constructors
    for(int i = 0; i < num; ++i) {
        Construct(&Element(elem + i));
    }

    return elem;
}

template< typename T, class A >
inline int CUtlVector<T, A>::InsertMultipleBefore(int elem, int num, const T *pToInsert)
{
    if(num == 0)
        return elem;

    // Can insert at the end
    assert((elem == Count()) || IsValidIndex(elem));

    GrowVector(num);
    ShiftElementsRight(elem, num);

    // Invoke default constructors
    if(!pToInsert) {
        for(int i = 0; i < num; ++i) {
            Construct(&Element(elem + i));
        }
    } else {
        for(int i = 0; i < num; i++) {
            CopyConstruct(&Element(elem + i), pToInsert[i]);
        }
    }

    return elem;
}


//-----------------------------------------------------------------------------
// Finds an element (element needs operator== defined)
//-----------------------------------------------------------------------------
template< typename T, class A >
int CUtlVector<T, A>::GetOffset(const T& src) const
{
    for(int i = 0; i < Count(); ++i) {
        if(Element(i) == src)
            return i;
    }
    return -1;
}

template< typename T, class A >
void CUtlVector<T, A>::FillWithValue(const T& src)
{
    for(int i = 0; i < Count(); i++) {
        Element(i) = src;
    }
}

template< typename T, class A >
bool CUtlVector<T, A>::HasElement(const T& src) const
{
    return (GetOffset(src) >= 0);
}


//-----------------------------------------------------------------------------
// Element removal
//-----------------------------------------------------------------------------
template< typename T, class A >
void CUtlVector<T, A>::FastRemove(int elem)
{
    assert(IsValidIndex(elem));

    Destruct(&Element(elem));
    if(m_Size > 0) {
        if(elem != m_Size - 1)
            memcpy(&Element(elem), &Element(m_Size - 1), sizeof(T));
        --m_Size;
    }
}

template< typename T, class A >
void CUtlVector<T, A>::Remove(int elem)
{
    Destruct(&Element(elem));
    ShiftElementsLeft(elem);
    --m_Size;
}

template< typename T, class A >
bool CUtlVector<T, A>::FindAndRemove(const T& src)
{
    int elem = GetOffset(src);
    if(elem != -1) {
        Remove(elem);
        return true;
    }
    return false;
}

template< typename T, class A >
bool CUtlVector<T, A>::FindAndFastRemove(const T& src)
{
    int elem = GetOffset(src);
    if(elem != -1) {
        FastRemove(elem);
        return true;
    }
    return false;
}

template< typename T, class A >
void CUtlVector<T, A>::RemoveMultiple(int elem, int num)
{
    assert(elem >= 0);
    assert(elem + num <= Count());

    for(int i = elem + num; --i >= elem; )
        Destruct(&Element(i));

    ShiftElementsLeft(elem, num);
    m_Size -= num;
}

template< typename T, class A >
void CUtlVector<T, A>::RemoveMultipleFromHead(int num)
{
    assert(num <= Count());

    for(int i = num; --i >= 0; )
        Destruct(&Element(i));

    ShiftElementsLeft(0, num);
    m_Size -= num;
}

template< typename T, class A >
void CUtlVector<T, A>::RemoveMultipleFromTail(int num)
{
    assert(num <= Count());

    for(int i = m_Size - num; i < m_Size; i++)
        Destruct(&Element(i));

    m_Size -= num;
}

template< typename T, class A >
void CUtlVector<T, A>::RemoveAll()
{
    for(int i = m_Size; --i >= 0; ) {
        Destruct(&Element(i));
    }

    m_Size = 0;
}


//-----------------------------------------------------------------------------
// Memory deallocation
//-----------------------------------------------------------------------------

template< typename T, class A >
inline void CUtlVector<T, A>::Purge()
{
    RemoveAll();
    m_Memory.Purge();
    ResetDbgInfo();
}


template< typename T, class A >
inline void CUtlVector<T, A>::PurgeAndDeleteElements()
{
    for(int i = 0; i < m_Size; i++) {
        delete Element(i);
    }
    Purge();
}

template< typename T, class A >
inline void CUtlVector<T, A>::Compact()
{
    m_Memory.Purge(m_Size);
}

template< typename T, class A >
inline int CUtlVector<T, A>::NumAllocated() const
{
    return m_Memory.NumAllocated();
}


//-----------------------------------------------------------------------------
// Data and memory validation
//-----------------------------------------------------------------------------
#ifdef DBGFLAG_VALIDATE
template< typename T, class A >
void CUtlVector<T, A>::Validate(CValidator &validator, char *pchName)
{
    validator.Push(typeid(*this).name(), this, pchName);

    m_Memory.Validate(validator, "m_Memory");

    validator.Pop();
}
#endif // DBGFLAG_VALIDATE

// A vector class for storing pointers, so that the elements pointed to by the pointers are deleted
// on exit.
template<class T> class CUtlVectorAutoPurge : public CUtlVector< T, CUtlMemory< T, int> >
{
public:
    ~CUtlVectorAutoPurge(void)
    {
        this->PurgeAndDeleteElements();
    }
};

// easy string list class with dynamically allocated strings. For use with V_SplitString, etc.
// Frees the dynamic strings in destructor.
class CUtlStringList : public CUtlVectorAutoPurge< char *>
{
public:
    void CopyAndAddToTail(char const *pString)			// clone the string and add to the end
    {
        char *pNewStr = new char[1 + strlen(pString)];
        strcpy_s(pNewStr, 1 + strlen(pString), pString);
        AddToTail(pNewStr);
    }

    static int __cdecl SortFunc(char * const * sz1, char * const * sz2)
    {
        return strcmp(*sz1, *sz2);
    }

};