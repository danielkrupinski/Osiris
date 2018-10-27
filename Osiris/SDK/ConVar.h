#pragma once

#include "UtlVector.h"

typedef void(*FnChangeCallback_t)(ConVar *var, const char *pOldValue, float flOldValue);

class ConCommandBaseAccessor {
public:
    virtual bool RegisterConCommandBase(ConCommandBase *pVar) = 0;
};

class ConCommandBase
{
    friend class CCvar;
    friend class ConVar;
    friend class ConCommand;
    friend void ConVar_Register(int nCVarFlag, ConCommandBaseAccessor *pAccessor);

    // FIXME: Remove when ConVar changes are done
    friend class CDefaultCvar;

public:
    ConCommandBase(void);
    ConCommandBase(const char *pName, const char *pHelpString = 0, int flags = 0);

    virtual                     ~ConCommandBase(void);
    virtual bool                IsCommand(void) const;
    virtual bool                IsFlagSet(int flag) const;
    virtual void                AddFlags(int flags);
    virtual void                RemoveFlags(int flags);
    virtual int                 GetFlags() const;
    virtual const char*         GetName(void) const;
    virtual const char*         GetHelpText(void) const;
    const ConCommandBase*       GetNext(void) const;
    ConCommandBase*             GetNext(void);
    virtual bool                IsRegistered(void) const;
    virtual int                 GetDLLIdentifier() const;

    virtual void                Create(const char *pName, const char *pHelpString = 0, int flags = 0);
    virtual void                Init();
    void                        Shutdown();
    char*                       CopyString(const char *from);

    ConCommandBase*             m_pNext;
    bool                        m_bRegistered;
    const char*                 m_pszName;
    const char*                 m_pszHelpString;
    int                         m_nFlags;

protected:
    static ConCommandBase* s_pConCommandBases;
    static ConCommandBaseAccessor* s_pAccessor;

public:
    static ConCommandBase* s_pRegisteredCommands;
};

class ConVar : public ConCommandBase, public ConVar {
    friend class CCvar;
    friend class ConVarRef;
    friend class SplitScreenConVarRef;

public:
    typedef ConCommandBase BaseClass;

    ConVar(const char *pName, const char *pDefaultValue, int flags = 0);

    ConVar(const char *pName, const char *pDefaultValue, int flags, const char *pHelpString);
    ConVar(const char *pName, const char *pDefaultValue, int flags, const char *pHelpString, bool bMin, float fMin, bool bMax, float fMax);
    ConVar(const char *pName, const char *pDefaultValue, int flags, const char *pHelpString, FnChangeCallback_t callback);
    ConVar(const char *pName, const char *pDefaultValue, int flags, const char *pHelpString, bool bMin, float fMin, bool bMax, float fMax, FnChangeCallback_t callback);

    virtual                     ~ConVar(void);
    virtual bool                IsFlagSet(int flag) const;
    virtual const char*         GetHelpText(void) const;
    virtual bool                IsRegistered(void) const;
    virtual const char*         GetName(void) const;
    virtual const char*         GetBaseName(void) const;
    virtual int                 GetSplitScreenPlayerSlot() const;

    virtual void                AddFlags(int flags);
    virtual int                 GetFlags() const;
    virtual bool                IsCommand(void) const;

    // Install a change callback (there shouldn't already be one....)
    void InstallChangeCallback(FnChangeCallback_t callback, bool bInvoke = true);
    void RemoveChangeCallback(FnChangeCallback_t callbackToRemove);

    int GetChangeCallbackCount() const { return m_pParent->m_fnChangeCallbacks.size; }
    FnChangeCallback_t GetChangeCallback(int slot) const { return m_pParent->m_fnChangeCallbacks[slot]; }

    // Retrieve value
    virtual float                   GetFloat(void) const;
    virtual int                     GetInt(void) const;
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
    void                            Revert(void);
    bool                            HasMin() const;
    bool                            HasMax() const;
    bool                            GetMin(float& minVal) const;
    bool                            GetMax(float& maxVal) const;
    float                           GetMinValue() const;
    float                           GetMaxValue() const;
    const char*                     GetDefault(void) const;

    struct CVValue_t
    {
        char*   m_pszString;
        int     m_StringLength;
        float   m_fValue;
        int     m_nValue;
    };

    CVValue_t &GetRawValue()
    {
        return m_Value;
    }

    const CVValue_t &GetRawValue() const
    {
        return m_Value;
    }

    //private:
    bool                        InternalSetColorFromString(const char *value);
    virtual void                InternalSetValue(const char *value);
    virtual void                InternalSetFloatValue(float fNewValue);
    virtual void                InternalSetIntValue(int nValue);
    virtual void                InternalSetColorValue(Color value);
    virtual bool                ClampValue(float& value);
    virtual void                ChangeStringValue(const char *tempVal, float flOldValue);
    virtual void                Create(const char *pName, const char *pDefaultValue, int flags = 0, const char *pHelpString = 0, bool bMin = false, float fMin = 0.0, bool bMax = false, float fMax = false, FnChangeCallback_t callback = 0);

    // Used internally by OneTimeInit to Initialize.
    virtual void                Init();

    //protected:
    ConVar*                     m_pParent;
    const char*                 m_pszDefaultValue;
    CVValue_t                   m_Value;
    bool                        m_bHasMin;
    float                       m_fMinVal;
    bool                        m_bHasMax;
    float                       m_fMaxVal;

    // Call this function when ConVar changes
    UtlVector<FnChangeCallback_t> m_fnChangeCallbacks;
};
