#pragma once

#include "UtlVector.h"

class ConVar;

typedef void(*FnChangeCallback_t)(ConVar *var, const char *pOldValue, float flOldValue);

class ConCommandBase;

class ConCommandBaseAccessor {
public:
    virtual bool RegisterConCommandBase(ConCommandBase *pVar) = 0;
};

class ConCommandBase {
public:
    virtual                     ~ConCommandBase(void) = 0;
    virtual bool                IsCommand(void) const = 0;
    virtual bool                IsFlagSet(int flag) const = 0;
    virtual void                AddFlags(int flags) = 0;
    virtual void                RemoveFlags(int flags) = 0;
    virtual int                 GetFlags() const = 0;
    virtual const char*         GetName(void) const = 0;
    virtual const char*         GetHelpText(void) const = 0;

    virtual bool                IsRegistered(void) const = 0;
    virtual int                 GetDLLIdentifier() const = 0;

    virtual void                Create(const char *pName, const char *pHelpString = 0, int flags = 0) = 0;
    virtual void                Init() = 0;
    int                         m_nFlags;
};

class ConVar : public ConCommandBase {
public:
    typedef ConCommandBase BaseClass;

    ConVar(const char *pName, const char *pDefaultValue, int flags = 0);

    ConVar(const char *pName, const char *pDefaultValue, int flags, const char *pHelpString);
    ConVar(const char *pName, const char *pDefaultValue, int flags, const char *pHelpString, bool bMin, float fMin, bool bMax, float fMax);
    ConVar(const char *pName, const char *pDefaultValue, int flags, const char *pHelpString, FnChangeCallback_t callback);
    ConVar(const char *pName, const char *pDefaultValue, int flags, const char *pHelpString, bool bMin, float fMin, bool bMax, float fMax, FnChangeCallback_t callback);

    virtual                     ~ConVar(void) = 0;
    virtual bool                IsFlagSet(int flag) const = 0;
    virtual const char*         GetHelpText(void) const = 0;
    virtual bool                IsRegistered(void) const = 0;
    virtual const char*         GetName(void) const = 0;
    virtual const char*         GetBaseName(void) const = 0;
    virtual int                 GetSplitScreenPlayerSlot() const;

    virtual void                AddFlags(int flags) = 0;
    virtual int                 GetFlags() const = 0;
    virtual bool                IsCommand(void) const;

    int GetChangeCallbackCount() const { return m_pParent->m_fnChangeCallbacks.size; }
    FnChangeCallback_t GetChangeCallback(int slot) const { return m_pParent->m_fnChangeCallbacks[slot]; }

    // Retrieve value
    virtual float                   GetFloat(void) const = 0;
    virtual int                     GetInt(void) const = 0;
    //Color          GetColor(void) const;
    //bool           GetBool() const { return !!GetInt(); }
    //char const*    GetString(void) const;

    // Compiler driven selection for template use
    template <typename T> T Get(void) const;
    template <typename T> T Get(T *) const;

    // Any function that allocates/frees memory needs to be virtual or else you'll have crashes
    //  from alloc/free across dll/exe boundaries.

    // These just call into the IConCommandBaseAccessor to check flags and Set the var (which ends up calling InternalSetValue).
    virtual void                    SetValue(const char *value);
    virtual void                    SetValue(float value);
    virtual void                    SetValue(int value);
    using Color = struct { float r, g, b, a; };
    virtual void                    SetValue(Color value);

    // Reset to default value

    //private:
    virtual void                InternalSetValue(const char *value) = 0;
    virtual void                InternalSetFloatValue(float fNewValue) = 0;
    virtual void                InternalSetIntValue(int nValue) = 0;
    virtual void                InternalSetColorValue(Color value) = 0;
    virtual bool                ClampValue(float& value);
    virtual void                ChangeStringValue(const char *tempVal, float flOldValue) = 0;
    virtual void                Create(const char *pName, const char *pDefaultValue, int flags = 0, const char *pHelpString = 0, bool bMin = false, float fMin = 0.0, bool bMax = false, float fMax = false, FnChangeCallback_t callback = 0) = 0;

    // Used internally by OneTimeInit to Initialize.
    virtual void                Init() = 0;

    struct CVValue_t
    {
        char*   m_pszString;
        int     m_StringLength;
        float   m_fValue;
        int     m_nValue;
    };

    //protected:
    ConVar*                     m_pParent;
    CVValue_t                   m_Value;
    bool                        m_bHasMin;
    float                       m_fMinVal;
    bool                        m_bHasMax;
    float                       m_fMaxVal;
    // Call this function when ConVar changes
    UtlVector<FnChangeCallback_t> m_fnChangeCallbacks;
};
