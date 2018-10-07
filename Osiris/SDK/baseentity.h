#pragma once

#include "offsets.h"
#include <array>
#include "usercmd.h"
#include "CBaseAnimState.h"

class IClientRenderable;
class IClientNetworkable;
class IClientUnknown;
class IClientThinkable;
struct model_t;
class CBaseCombatWeapon;
class ClientClass;
#define ACTIVITY_NOT_AVAILABLE		-1

static const char *Ranks[] =
{
    "Unranked",
    "Silver I",
    "Silver II",
    "Silver III",
    "Silver IV",
    "Silver Elite",
    "Silver Elite Master",

    "pWhite Nova I",
    "pWhite Nova II",
    "pWhite Nova III",
    "pWhite Nova Master",
    "Master Guardian I",
    "Master Guardian II",

    "Master Guardian Elite",
    "Distinguished Master Guardian",
    "Legendary Eagle",
    "Legendary Eagle Master",
    "Supreme Master First Class",
    "The Global Elite"
};
class CollisionProperty
{
public:
    Vector VecMins()
    {
        return *reinterpret_cast<Vector*>((DWORD)this + 0x8);
    }
    Vector VecMaxs()
    {
        return *reinterpret_cast<Vector*>((DWORD)this + 0x14);
    }
};


typedef unsigned short ClientShadowHandle_t;
typedef unsigned short ClientRenderHandle_t;
typedef unsigned short ModelInstanceHandle_t;
typedef unsigned char uint8;
class IClientRenderable
{
public:
    virtual IClientUnknown*            GetIClientUnknown() = 0;
    virtual Vector const&              GetRenderOrigin(void) = 0;
    virtual QAngle const&              GetRenderAngles(void) = 0;
    virtual bool                       ShouldDraw(void) = 0;
    virtual int                        GetRenderFlags(void) = 0; // ERENDERFLAGS_xxx
    virtual void                       Unused(void) const {}
    virtual ClientShadowHandle_t       GetShadowHandle() const = 0;
    virtual ClientRenderHandle_t&      RenderHandle() = 0;
    virtual const model_t*             GetModel() const = 0;
    virtual int                        DrawModel(int flags, const int /*RenderableInstance_t*/ &instance) = 0;
    virtual int                        GetBody() = 0;
    virtual void                       GetColorModulation(float* color) = 0;
    virtual bool                       LODTest() = 0;
    virtual bool                       SetupBones(matrix3x4_t *pBoneToWorldOut, int nMaxBones, int boneMask, float currentTime) = 0;
    virtual void                       SetupWeights(const matrix3x4_t *pBoneToWorld, int nFlexWeightCount, float *pFlexWeights, float *pFlexDelayedWeights) = 0;
    virtual void                       DoAnimationEvents(void) = 0;
    virtual void* /*IPVSNotify*/       GetPVSNotifyInterface() = 0;
    virtual void                       GetRenderBounds(Vector& mins, Vector& maxs) = 0;
    virtual void                       GetRenderBoundsWorldspace(Vector& mins, Vector& maxs) = 0;
    virtual void                       GetShadowRenderBounds(Vector &mins, Vector &maxs, int /*ShadowType_t*/ shadowType) = 0;
    virtual bool                       ShouldReceiveProjectedTextures(int flags) = 0;
    virtual bool                       GetShadowCastDistance(float *pDist, int /*ShadowType_t*/ shadowType) const = 0;
    virtual bool                       GetShadowCastDirection(Vector *pDirection, int /*ShadowType_t*/ shadowType) const = 0;
    virtual bool                       IsShadowDirty() = 0;
    virtual void                       MarkShadowDirty(bool bDirty) = 0;
    virtual IClientRenderable*         GetShadowParent() = 0;
    virtual IClientRenderable*         FirstShadowChild() = 0;
    virtual IClientRenderable*         NextShadowPeer() = 0;
    virtual int /*ShadowType_t*/       ShadowCastType() = 0;
    virtual void                       CreateModelInstance() = 0;
    virtual ModelInstanceHandle_t      GetModelInstance() = 0;
    virtual const matrix3x4_t&         RenderableToWorldTransform() = 0;
    virtual int                        LookupAttachment(const char *pAttachmentName) = 0;
    virtual   bool                     GetAttachment(int number, Vector &origin, QAngle &angles) = 0;
    virtual bool                       GetAttachment(int number, matrix3x4_t &matrix) = 0;
    virtual float*                     GetRenderClipPlane(void) = 0;
    virtual int                        GetSkin() = 0;
    virtual void                       OnThreadedDrawSetup() = 0;
    virtual bool                       UsesFlexDelayedWeights() = 0;
    virtual void                       RecordToolMessage() = 0;
    virtual bool                       ShouldDrawForSplitScreenUser(int nSlot) = 0;
    virtual uint8                      OverrideAlphaModulation(uint8 nAlpha) = 0;
    virtual uint8                      OverrideShadowAlphaModulation(uint8 nAlpha) = 0;
    bool SetupBones(VMatrix *pBoneToWorldOut, int nMaxBones, int boneMask, float currentTime)
    {
        typedef bool(__thiscall* oSetupBones)(PVOID, VMatrix *, int, int, float);
        return CallVFunction< oSetupBones>(this, 13)(this, pBoneToWorldOut, nMaxBones, boneMask, currentTime);
    }
    const model_t* GetModel() //void*
    {
        typedef const model_t*(__thiscall* OriginalFn)(PVOID);
        return CallVFunction<OriginalFn>(this, 8)(this);
    }
};

class IClientNetworkable
{
public:
    virtual IClientUnknown*	GetIClientUnknown() = 0;
    virtual void			Release() = 0;
    virtual void*			GetClientClass() = 0;// FOR NETVARS FIND YOURSELF ClientClass* stuffs
    virtual void             NotifyShouldTransmit(int state) = 0;
    virtual void             OnPreDataChanged(int updateType) = 0;
    virtual void             OnDataChanged(int updateType) = 0;
    virtual void             PreDataUpdate(int updateType) = 0;
    virtual void             PostDataUpdate(int updateType) = 0;
    virtual void			unknown();
    virtual bool			IsDormant(void) = 0;
    virtual int				Index(void) const = 0;
    virtual void			ReceiveMessage(int classID /*, bf_read &msg*/) = 0;
    virtual void*			GetDataTableBasePtr() = 0;
    virtual void			SetDestroyedOnRecreateEntities(void) = 0;

};
class IClientUnknown
{
public:
    virtual CollisionProperty*		GetCollideable() = 0;
    virtual IClientNetworkable*	GetClientNetworkable() = 0;
    virtual IClientRenderable*	GetClientRenderable() = 0;
    virtual void*		GetIClientCBaseEntity() = 0;
    virtual void*		GetBaseCBaseEntity() = 0;
    virtual IClientThinkable*	GetClientThinkable() = 0;
};
class IClientThinkable
{
public:
    virtual IClientUnknown*		GetIClientUnknown() = 0;
    virtual void				ClientThink() = 0;
    virtual void*				GetThinkHandle() = 0;
    virtual void				SetThinkHandle(void* hThink) = 0;
    virtual void				Release() = 0;
};

class VarMapEntry_t
{
public:
    unsigned short		type;
    unsigned short		m_bNeedsToInterpolate;	// Set to false when this var doesn't
                                                // need Interpolate() called on it anymore.
    void				*data;
    void	*watcher;
};

struct VarMapping_t
{
    CUtlVector<VarMapEntry_t>	m_Entries;
    int							m_nInterpolatedEntries;
    float						m_lastInterpolationTime;
};

class CBaseAnimating
{
public:

    enum
    {
        NUM_POSEPAREMETERS = 24,
        NUM_BONECTRLS = 4
    };



};

class CAnimationLayer
{
public:
    char  pad_0000[20];
    uint32_t m_nOrder; //0x0014
    uint32_t m_nSequence; //0x0018
    float_t m_flPrevCycle; //0x001C
    float_t m_flWeight; //0x0020
    float_t m_flWeightDeltaRate; //0x0024
    float_t m_flPlaybackRate; //0x0028
    float_t m_flCycle; //0x002C
    void *m_pOwner; //0x0030 // player's thisptr
    char  pad_0038[4]; //0x0034
}; //Size: 0x0038

template <typename T>
__forceinline T Member(void *base, DWORD offset = 0)
{
    return (T)((DWORD)base + offset);
}


struct inputdata_t;
typedef enum _fieldtypes
{
    FIELD_VOID = 0,			// No type or value
    FIELD_FLOAT,			// Any floating point value
    FIELD_STRING,			// A string ID (return from ALLOC_STRING)
    FIELD_VECTOR,			// Any vector, QAngle, or AngularImpulse
    FIELD_QUATERNION,		// A quaternion
    FIELD_INTEGER,			// Any integer or enum
    FIELD_BOOLEAN,			// boolean, implemented as an int, I may use this as a hint for compression
    FIELD_SHORT,			// 2 byte integer
    FIELD_CHARACTER,		// a byte
    FIELD_COLOR32,			// 8-bit per channel r,g,b,a (32bit color)
    FIELD_EMBEDDED,			// an embedded object with a datadesc, recursively traverse and embedded class/structure based on an additional typedescription
    FIELD_CUSTOM,			// special type that contains function pointers to it's read/write/parse functions

    FIELD_CLASSPTR,			// CBaseEntity *
    FIELD_EHANDLE,			// Entity handle
    FIELD_EDICT,			// edict_t *

    FIELD_POSITION_VECTOR,	// A world coordinate (these are fixed up across level transitions automagically)
    FIELD_TIME,				// a floating point time (these are fixed up automatically too!)
    FIELD_TICK,				// an integer tick count( fixed up similarly to time)
    FIELD_MODELNAME,		// Engine string that is a model name (needs precache)
    FIELD_SOUNDNAME,		// Engine string that is a sound name (needs precache)

    FIELD_INPUT,			// a list of inputed data fields (all derived from CMultiInputVar)
    FIELD_FUNCTION,			// A class function pointer (Think, Use, etc)

    FIELD_VMATRIX,			// a vmatrix (output coords are NOT worldspace)

                            // NOTE: Use float arrays for local transformations that don't need to be fixed up.
                            FIELD_VMATRIX_WORLDSPACE,// A VMatrix that maps some local space to world space (translation is fixed up on level transitions)
                            FIELD_MATRIX3X4_WORLDSPACE,	// matrix3x4_t that maps some local space to world space (translation is fixed up on level transitions)

                            FIELD_INTERVAL,			// a start and range floating point interval ( e.g., 3.2->3.6 == 3.2 and 0.4 )
                            FIELD_MODELINDEX,		// a model index
                            FIELD_MATERIALINDEX,	// a material index (using the material precache string table)

                            FIELD_VECTOR2D,			// 2 floats

                            FIELD_TYPECOUNT,		// MUST BE LAST
} fieldtype_t;

class ISaveRestoreOps;
class C_BaseEntity;
//
// Function prototype for all input handlers.
//
typedef void (C_BaseEntity::*inputfunc_t)(inputdata_t &data);

struct datamap_t;
class typedescription_t;

enum
{
    TD_OFFSET_NORMAL = 0,
    TD_OFFSET_PACKED = 1,

    // Must be last
    TD_OFFSET_COUNT,
};

class typedescription_t
{
public:
    int32_t fieldType; //0x0000
    char* fieldName; //0x0004
    int fieldOffset[TD_OFFSET_COUNT]; //0x0008
    int16_t fieldSize_UNKNWN; //0x0010
    int16_t flags_UNKWN; //0x0012
    char pad_0014[12]; //0x0014
    datamap_t* td; //0x0020
    char pad_0024[24]; //0x0024
}; //Size: 0x003C


   //-----------------------------------------------------------------------------
   // Purpose: stores the list of objects in the hierarchy
   //            used to iterate through an object's data descriptions
   //-----------------------------------------------------------------------------
struct datamap_t
{
    typedescription_t    *dataDesc;
    int                    dataNumFields;
    char const            *dataClassName;
    datamap_t            *baseMap;

    bool                chains_validated;
    // Have the "packed" offsets been computed
    bool                packed_offsets_computed;
    int                    packed_size;
};


typedef unsigned long CBaseHandle;
enum DataUpdateType_txd
{
    DATA_UPDATE_CREATEDxd = 0,
    DATA_UPDATE_DATATABLE_CHANGEDxd,
};
class CBaseEntity
{
public:


    float GetNextAttack()
    {
        return *reinterpret_cast<float*>(uint32_t(this) + offys.m_flNextAttack);
    }



    CBaseHandle GetObserverTarget() {
        if (!this)
            return NULL;

        return *reinterpret_cast<CBaseHandle*>(uintptr_t(this) + offys.m_hObserverTarget);
    }

    Vector GetVecOrigin()
    {
        return *(Vector*)((DWORD)this + 0x134);
    }
    datamap_t * GetPredDescMap()
    {
        typedef datamap_t*(__thiscall *o_GetPredDescMap)(void*);
        return CallVFunction<o_GetPredDescMap>(this, 17)(this);
    }

    CBaseHandle* m_hMyWeapons()
    {
        return (CBaseHandle*)((uintptr_t)this + 0x2DE8);
    }
    void SetModelIndex(int nModelIndex);

    void PreDataUpdate(DataUpdateType_txd updateType);
    bool IsPlayer();
    float_t & m_flMaxspeed();
    float_t & m_surfaceFriction();
    UINT PhysicsSolidMaskForEntity()
    {
        typedef UINT(__thiscall* Fn)(void*);
        return CallVFunction<Fn>(this, 148)(this);
    }
    UINT* GetWearables()
    {
        return reinterpret_cast<UINT*>((DWORD)this + offys.m_hMyWearables);
    }
    int GetChockedPackets();
    float GetOldSimulationTime();

    Vector& m_vecNetworkOrigin();
    bool IsTargettingLocal();
    //virtual IClientUnknown*		GetIClientUnknown();
    //{
    //	return (IClientUnknown*)this; //index0
    //}
    int GetSequenceActivity(int sequence);

    void SetCurrentCommand(CUserCmd * cmd);

    void SetAbsAngles(QAngle angles);

    void InvalidateBoneCache();

    bool IsValidRenderable();

    void SetAbsOrigin(const Vector & origin);

    int draw_model(int flags, uint8_t alpha) {
        using fn = int(__thiscall*)(void*, int, uint8_t);
        return CallVFunction< fn >(GetRenderable(), 9)(GetRenderable(), flags, alpha);
    }

    bool IsValidTarget();

    void SetAngle2(Vector wantedang);

    bool canHit(Vector end, CBaseEntity * ent);

    Vector GetBonePos(int i);

    AnimationLayer& GetAnimOverlay(int Index)
    {
        return (*(AnimationLayer**)((DWORD)this + 0x2970))[Index];
    }
    AnimationLayer* GetAnimOverlays()
    {
        // to find offset: use 9/12/17 dll
        // sig: 55 8B EC 51 53 8B 5D 08 33 C0
        return *(AnimationLayer**)((DWORD)this + 10608);
    }
    int GetNumAnimOverlays()
    {
        return *(int*)((DWORD)this + 0x297C);
    }
    std::array<float, 24> &m_flPoseParameter()
    {
        static int _m_flPoseParameter = offys.m_flPoseParameter;
        return *(std::array<float, 24>*)((uintptr_t)this + _m_flPoseParameter);
    }
    CBaseAnimating*		GetBaseAnimating()
    {
        typedef CBaseAnimating*& (__thiscall* OriginalFn)(void*);
        return CallVFunction<OriginalFn>(this, 1)(this);
    } //vtable index 1
    float GetUpdateTime()
    {
        return *reinterpret_cast<int*>((DWORD)this + 0xA9FC + 0x8);
    }
    float *GetPoseParameterss()
    {
        return reinterpret_cast<float*>((DWORD)this + 0x2764);

    }
    float *LowerBodyYaws()
    {
        return reinterpret_cast<float*>((DWORD)this + offys.m_flLowerBodyYawTarget);
    }
    int *GetSequences()
    {
        return reinterpret_cast<int*>((DWORD)this + offys.m_nSequence);
    }
    float *GetCycles()
    {
        return reinterpret_cast<float*>((DWORD)this + offys.m_flCycle);
    }
    int GetSequence()
    {
        return *reinterpret_cast<int*>((DWORD)this + offys.m_nSequence);
    }
    float *GetSimulationTimes()
    {
        return reinterpret_cast<float*>((DWORD)this + offys.m_flSimulationTime);
    }
    float GetCycle()
    {
        return *reinterpret_cast<int*>((DWORD)this + offys.m_flCycle);
    }
    void UpdateClientSideAnimation()
    {
        typedef void(__thiscall* Fn)(void*);
        return CallVFunction<Fn>(this, 218)(this);
    }
    float LowerBodyYaw()
    {
        return *reinterpret_cast<float*>((DWORD)this + offys.m_flLowerBodyYawTarget);
    }



    float *FlashMaxAlpha()
    {
        return reinterpret_cast<float*>((DWORD)this + offys.m_flFlashMaxAlpha);
    }
    float GetMaxSpeed()
    {
        return *reinterpret_cast<float*>((DWORD)this + offys.m_flMaxSpeed);
    }
    float GetAnimationTime()
    {
        return *reinterpret_cast<float*>((DWORD)this + /*offys.m_flAnimTime*/0x3C);
    }
    float GetSimulationTime()
    {
        return *reinterpret_cast<float*>((DWORD)this + offys.m_flSimulationTime);
    }
    int &GetTickBase(void)
    {
        return *(int*)((DWORD)this + offys.m_nTickBase);
    }
    int GetHitboxSet()
    {
        return *reinterpret_cast<int*>((DWORD)this + 0x09FC);
    }
    Vector GetAngRotation()
    {
        return *reinterpret_cast<Vector*>((DWORD)this + 0x0128);
    }
    Vector GetPredictedEyePosition(float interval_per_tick)
    {
        return GetEyePosition() + (GetVelocity() * interval_per_tick);
    }
    float Friction()
    {
        return *reinterpret_cast<float*>((DWORD)this + 0x0140);
    }
    int GetTeamNum()
    {
        return *reinterpret_cast<int*>((DWORD)this + 0xF0);
    }
    int GetArmor()
    {
        return *reinterpret_cast<int*>((DWORD)this + 0xB24C);
    }

    char* GetArmorName()
    {
        if (GetArmor() > 0)
        {
            if (HasHelmet())
                return "HK";
            else
                return "K";
        }
        else
            return " ";
    }
    bool m_bHasHeavyArmor()
    {
        return *(bool*)((DWORD)this + offys.m_bHasHeavyArmor);
    }
    bool HasHelmet()
    {
        return *reinterpret_cast<bool*>((DWORD)this + offys.m_bHasHelmet);
    }
    Vector Velocity()
    {
        return *reinterpret_cast<Vector*>((DWORD)this + 0x0110);
    }

    int GetHealth()
    {
        return *reinterpret_cast<int*>((DWORD)this + 0xFC);
    }

    int Index()
    {
        return GetNetworkable()->Index();
    }
    VarMapping_t* GetVarMap()
    {
        return reinterpret_cast<VarMapping_t*>((DWORD)this + 0x24);
    }
    int* GetFlags()
    {
        return (int*)((DWORD)this + offys.m_fFlags);
    }
    int GetFlagsxd()
    {
        return *reinterpret_cast<int*>(uintptr_t(this) + offys.m_fFlags);
    }
    CBaseAnimState* GetAnimState()
    {
        return *reinterpret_cast<CBaseAnimState**>(uintptr_t(this) + offys.animstate);
    }
    float GetFriction()
    {
        return *reinterpret_cast<float*>(uintptr_t(this + offys.m_flFriction));
    }
    float GetFallVelocity()
    {
        return *reinterpret_cast<float*>(uintptr_t(this + offys.m_flFallVelocity));
    }
    int GetShotsFired()
    {
        return *reinterpret_cast<int*>((DWORD)this + offys.m_iShotsFired);
    }
    Vector GetAbsOrigin()
    {
        return *(Vector*)((DWORD)this + 0x84 + 0x1C);
    }
    bool IsMoving()
    {
        if (this->GetVelocity().Length() > 0.1f)
            return true;
        return false;
    }
    Vector GetEyePosition(void)
    {
        return GetAbsOrigin() + *(Vector*)((DWORD)this + offys.m_vecViewOffset);
    }
    Vector GetOrigin()
    {
        return *reinterpret_cast<Vector*>((DWORD)this + offys.m_vecOrigin);
    }
    float GetStepSize()
    {
        return *reinterpret_cast<float*>(uintptr_t(this + offys.m_flStepSize));
    }
    Vector SetOrigin(Vector Origin)
    {
        *(Vector*)((DWORD)this + offys.m_vecOrigin) = Origin;
    }
    Vector GetViewOffset()
    {
        return *reinterpret_cast<Vector*>((DWORD)this + 0x0104);;
    }
    Vector GetBaseVelocity()
    {
        return *reinterpret_cast<Vector*>((DWORD)this + offys.m_vecBaseVelocity);
    }
    void SetBaseVelocity(Vector Velocity)
    {
        *(Vector*)((DWORD)this + offys.m_vecBaseVelocity) = Velocity;
    }
    void SetVelocity(Vector Velocity)
    {
        *(Vector*)((DWORD)this + offys.m_vecVelocity) = Velocity;
    }
    void SetFallVelocity(float FallVelocity)
    {
        *(float*)((DWORD)this + offys.m_flFallVelocity) = FallVelocity;
    }
    Vector& GetAbsAngles()
    {
        typedef Vector& (__thiscall* OriginalFn)(void*);
        return CallVFunction<OriginalFn>(this, 11)(this);
        //return *reinterpret_cast<Vector*>((DWORD)this + 0x0128);
    }

    void* GetClientClass()
    {
        void* Networkable = (void*)(this + 0x8);
        typedef void* (__thiscall* OriginalFn)(void*);
        return CallVFunction<OriginalFn>(Networkable, 2)(Networkable);
    }
    ClientClass* GetClientClassxd()
    {
        void* Networkable = (void*)(this + 0x8);
        typedef ClientClass*(__thiscall* OriginalFn)(void*);
        return CallVFunction<OriginalFn>(Networkable, 2)(Networkable);
    }

    bool UnkAwCeck()
    {
        typedef bool* (__thiscall* OriginalFn)(void*);
        return CallVFunction<OriginalFn>(this, 152)(this);
    }
    Vector GetVelocity()
    {
        return *reinterpret_cast<Vector*>((DWORD)this + offys.m_vecVelocity);
    }
    bool IsDormant()
    {
        return *reinterpret_cast<bool*>((DWORD)this + 0xE9);
    }

    float MaxSpeed()
    {
        return *reinterpret_cast<float*>((DWORD)this + 0x3240);
    }

    bool isAlive()
    {
        BYTE lifestate = *(BYTE*)((DWORD)this + 0x025B);
        return (lifestate == 0);
    }
    bool IsWeapon()
    {
        typedef bool(__thiscall* Fn)(void*);
        return CallVFunction<Fn>(this, 160)(this);
    }
    int GetIndex()
    {
        //	void* networkable = (void*)(this + 0x8);
        //typedef int(__thiscall* OriginalFn)(PVOID);
        //return CallVFunction<OriginalFn>(this, 76)(this);
        return GetNetworkable()->Index();
    }
    IClientNetworkable* GetNetworkable()
    {
        return reinterpret_cast<IClientNetworkable*>((DWORD)this + 0x8);
    }

    bool SetupBones(VMatrix *pBoneToWorldOut, int nMaxBones, int boneMask, float currentTime);

    bool SetupBones2(matrix3x4_t *pBoneToWorldOut, int nMaxBones, int boneMask, float currentTime);


    const model_t* GetModel() //void*
    {
        void *pRenderable = (void*)(this + 0x4);
        typedef const model_t*(__thiscall* OriginalFn)(PVOID);
        return CallVFunction<OriginalFn>(pRenderable, 8)(pRenderable);
    }
    float GetPoseParameter()
    {
        return *reinterpret_cast<float*>((DWORD)this + 0x2764);

    }
    int GetCollisionGroup()
    {
        return *reinterpret_cast<int*>((DWORD)this + offys.m_CollisionGroup);
        //GetNetVar 
    }
    bool IsBroken()
    {
        return *reinterpret_cast<bool*>((DWORD)this + 0x0A04);
    }
    CollisionProperty* GetCollision()
    {
        return reinterpret_cast<CollisionProperty*>((DWORD)this + 0x0318);
    }

    VMatrix& GetCollisionBoundTrans()
    {
        return *reinterpret_cast<VMatrix*>((DWORD)this + 0x0440);
    }
    IClientRenderable* GetRenderable()
    {
        return reinterpret_cast<IClientRenderable*>((DWORD)this + 0x4);
    }
    bool IsScoped()
    {
        return *reinterpret_cast<bool*>((DWORD)this + 0x388E);
    }
    bool IsProtected(void)
    {
        return *(bool*)((DWORD)this + offys.m_bGunGameImmunity);
    }
    QAngle GetPunchAngle()
    {
        return *reinterpret_cast<QAngle*>((DWORD)this + 0x0000301C);
    }
    QAngle GetPunchAngleVelocity()
    {
        return *reinterpret_cast<QAngle*>((DWORD)this + 0x3020);
    }
    QAngle* GetPunchAnglePtr()
    {
        return reinterpret_cast<QAngle*>((DWORD)this + 0x301C);
    }
    QAngle GetViewPunch()
    {
        return *reinterpret_cast<QAngle*>((DWORD)this + 0x3010);
    }
    QAngle* GetViewPunchPtr()
    {
        return reinterpret_cast<QAngle*>((DWORD)this + 0x3010);
    }
    Vector* GetEyeAnglesPtr()
    {
        return reinterpret_cast<Vector*>((DWORD)this + offys.m_angEyeAngles);
    }
    Vector GetEyeAngles()
    {
        return *reinterpret_cast<Vector*>((DWORD)this + offys.m_angEyeAngles);
    }
    void SetEyeAngles(Vector Angles)
    {
        *(Vector*)((uintptr_t)this + 0xAA08) = Angles;
    }
    CBaseCombatWeapon* GetWeapon();
    int GetActiveWeaponIndex();
    int GetGlowIndex()
    {
        return *reinterpret_cast<int*>((DWORD)this + 0x0000A320);
    }
    UINT* GetWeapons()
    {
        return (UINT*)((DWORD)this + 0x00002DE8);
    }
    int GetMoveType()
    {
        return *reinterpret_cast<int*>((DWORD)this + 0x258);
    }
    Vector GetNetworkOrigin()
    {
        return *reinterpret_cast<Vector*>((DWORD)this + offys.m_flFriction - sizeof(Vector));
    }
    int GetRank()
    {

        return *(int*)((DWORD)offys.dw_CSPlayerResource + offys.m_iCompetetiveRanking + this->Index() * 4);
    }
    const char* szRank()
    {
        return Ranks[GetRank()];
    }
    int GetWins()
    {
        return *(int*)((DWORD)offys.dw_CSPlayerResource + offys.m_iCompetetiveWins + this->Index() * 4);
    }
    int GetPing()
    {
        int size = this->Index() * sizeof(int);
        return *(int*)(uintptr_t(offys.dw_CSPlayerResource + offys.m_iPing + size));
    }
    int GetMoney()
    {
        return *reinterpret_cast<int*>((DWORD)this + 0xB228);
    }
    std::array<float, 24> GetPoseParameters()
    {
        std::array<float, 24> flParam;
        float * flPose = (float*)((DWORD)this + offys.m_flPoseParameter);
        for (int i = 0; i < 23; i++)
        {
            flParam[i] = flPose[i];
        }
        return flParam;
    }
};
enum class CSClasses // Must specify CSClasses::Whatever in order to use smth from here
                     // Last updated -- 29 November 2016 (Glove Update)
{
    CAI_BaseNPC = 0,
    CAK47 = 1,
    CBaseAnimating = 2,
    CBaseAnimatingOverlay = 3,
    CBaseAttributableItem = 4,
    CBaseButton = 5,
    CBaseCombatCharacter = 6,
    CBaseCombatWeapon = 7,
    CBaseCSGrenade = 8,
    CBaseCSGrenadeProjectile = 9,
    CBaseDoor = 10,
    CBaseEntity = 11,
    CBaseFlex = 12,
    CBaseGrenade = 13,
    CBaseParticleCBaseEntity = 14,
    CBasePlayer = 15,
    CBasePropDoor = 16,
    CBaseTeamObjectiveResource = 17,
    CBaseTempCBaseEntity = 18,
    CBaseToggle = 19,
    CBaseTrigger = 20,
    CBaseViewModel = 21,
    CBaseVPhysicsTrigger = 22,
    CBaseWeaponWorldModel = 23,
    CBeam = 24,
    CBeamSpotlight = 25,
    CBoneFollower = 26,
    CBreakableProp = 27,
    CBreakableSurface = 28,
    CC4 = 29,
    CCascadeLight = 30,
    CChicken = 31,
    CColorCorrection = 32,
    CColorCorrectionVolume = 33,
    CCSGameRulesProxy = 34,
    CCSPlayer = 35,
    CCSPlayerResource = 36,
    CCSRagdoll = 37,
    CCSTeam = 38,
    CDEagle = 39,
    CDecoyGrenade = 40,
    CDecoyProjectile = 41,
    CDynamicLight = 42,
    CDynamicProp = 43,
    CEconCBaseEntity = 44,
    CEconWearable = 45,
    CEmbers = 46,
    CCBaseEntityDissolve = 47,
    CCBaseEntityFlame = 48,
    CCBaseEntityFreezing = 49,
    CCBaseEntityParticleTrail = 50,
    CEnvAmbientLight = 51,
    CEnvDetailController = 52,
    CEnvDOFController = 53,
    CEnvParticleScript = 54,
    CEnvProjectedTexture = 55,
    CEnvQuadraticBeam = 56,
    CEnvScreenEffect = 57,
    CEnvScreenOverlay = 58,
    CEnvTonemapController = 59,
    CEnvWind = 60,
    CFEPlayerDecal = 61,
    CFireCrackerBlast = 62,
    CFireSmoke = 63,
    CFireTrail = 64,
    CFish = 65,
    CFlashbang = 66,
    CFogController = 67,
    CFootstepControl = 68,
    CFunc_Dust = 69,
    CFunc_LOD = 70,
    CFuncAreaPortalWindow = 71,
    CFuncBrush = 72,
    CFuncConveyor = 73,
    CFuncLadder = 74,
    CFuncMonitor = 75,
    CFuncMoveLinear = 76,
    CFuncOccluder = 77,
    CFuncReflectiveGlass = 78,
    CFuncRotating = 79,
    CFuncSmokeVolume = 80,
    CFuncTrackTrain = 81,
    CGameRulesProxy = 82,
    CHandleTest = 83,
    CHEGrenade = 84,
    CHostage = 85,
    CHostageCarriableProp = 86,
    CIncendiaryGrenade = 87,
    CInferno = 88,
    CInfoLadderDismount = 89,
    CInfoOverlayAccessor = 90,
    CItem_Healthshot = 91,
    CKnife = 92,
    CKnifeGG = 93,
    CLightGlow = 94,
    CMaterialModifyControl = 95,
    CMolotovGrenade = 96,
    CMolotovProjectile = 97,
    CMovieDisplay = 98,
    CParticleFire = 99,
    CParticlePerformanceMonitor = 100,
    CParticleSystem = 101,
    CPhysBox = 102,
    CPhysBoxMultiplayer = 103,
    CPhysicsProp = 104,
    CPhysicsPropMultiplayer = 105,
    CPhysMagnet = 106,
    CPlantedC4 = 107,
    CPlasma = 108,
    CPlayerResource = 109,
    CPointCamera = 110,
    CPointCommentaryNode = 111,
    CPoseController = 112,
    CPostProcessController = 113,
    CPrecipitation = 114,
    CPrecipitationBlocker = 115,
    CPredictedViewModel = 116,
    CProp_Hallucination = 117,
    CPropDoorRotating = 118,
    CPropJeep = 119,
    CPropVehicleDriveable = 120,
    CRagdollManager = 121,
    CRagdollProp = 122,
    CRagdollPropAttached = 123,
    CRopeKeyframe = 124,
    CSCAR17 = 125,
    CSceneCBaseEntity = 126,
    CSensorGrenade = 127,
    CSensorGrenadeProjectile = 128,
    CShadowControl = 129,
    CSlideshowDisplay = 130,
    CSmokeGrenade = 131,
    CSmokeGrenadeProjectile = 132,
    CSmokeStack = 133,
    CSpatialCBaseEntity = 134,
    CSpotlightEnd = 135,
    CSprite = 136,
    CSpriteOriented = 137,
    CSpriteTrail = 138,
    CStatueProp = 139,
    CSteamJet = 140,
    CSun = 141,
    CSunlightShadowControl = 142,
    CTeam = 143,
    CTeamplayRoundBasedRulesProxy = 144,
    CTEArmorRicochet = 145,
    CTEBaseBeam = 146,
    CTEBeamEntPoint = 147,
    CTEBeamEnts = 148,
    CTEBeamFollow = 149,
    CTEBeamLaser = 150,
    CTEBeamPoints = 151,
    CTEBeamRing = 152,
    CTEBeamRingPoint = 153,
    CTEBeamSpline = 154,
    CTEBloodSprite = 155,
    CTEBloodStream = 156,
    CTEBreakModel = 157,
    CTEBSPDecal = 158,
    CTEBubbles = 159,
    CTEBubbleTrail = 160,
    CTEClientProjectile = 161,
    CTEDecal = 162,
    CTEDust = 163,
    CTEDynamicLight = 164,
    CTEEffectDispatch = 165,
    CTEEnergySplash = 166,
    CTEExplosion = 167,
    CTEFireBullets = 168,
    CTEFizz = 169,
    CTEFootprintDecal = 170,
    CTEFoundryHelpers = 171,
    CTEGaussExplosion = 172,
    CTEGlowSprite = 173,
    CTEImpact = 174,
    CTEKillPlayerAttachments = 175,
    CTELargeFunnel = 176,
    CTEMetalSparks = 177,
    CTEMuzzleFlash = 178,
    CTEParticleSystem = 179,
    CTEPhysicsProp = 180,
    CTEPlantBomb = 181,
    CTEPlayerAnimEvent = 182,
    CTEPlayerDecal = 183,
    CTEProjectedDecal = 184,
    CTERadioIcon = 185,
    CTEShatterSurface = 186,
    CTEShowLine = 187,
    CTesla = 188,
    CTESmoke = 189,
    CTESparks = 190,
    CTESprite = 191,
    CTESpriteSpray = 192,
    CTest_ProxyToggle_Networkable = 193,
    CTestTraceline = 194,
    CTEWorldDecal = 195,
    CTriggerPlayerMovement = 196,
    CTriggerSoundOperator = 197,
    CVGuiScreen = 198,
    CVoteController = 199,
    CWaterBullet = 200,
    CWaterLODControl = 201,
    CWeaponAug = 202,
    CWeaponAWP = 203,
    CWeaponBaseItem = 204,
    CWeaponBizon = 205,
    CWeaponCSBase = 206,
    CWeaponCSBaseGun = 207,
    CWeaponCycler = 208,
    CWeaponElite = 209,
    CWeaponFamas = 210,
    CWeaponFiveSeven = 211,
    CWeaponG3SG1 = 212,
    CWeaponGalil = 213,
    CWeaponGalilAR = 214,
    CWeaponGlock = 215,
    CWeaponHKP2000 = 216,
    CWeaponM249 = 217,
    CWeaponM3 = 218,
    CWeaponM4A1 = 219,
    CWeaponMAC10 = 220,
    CWeaponMag7 = 221,
    CWeaponMP5Navy = 222,
    CWeaponMP7 = 223,
    CWeaponMP9 = 224,
    CWeaponNegev = 225,
    CWeaponNOVA = 226,
    CWeaponP228 = 227,
    CWeaponP250 = 228,
    CWeaponP90 = 229,
    CWeaponSawedoff = 230,
    CWeaponSCAR20 = 231,
    CWeaponScout = 232,
    CWeaponSG550 = 233,
    CWeaponSG552 = 234,
    CWeaponSG556 = 235,
    CWeaponSSG08 = 236,
    CWeaponTaser = 237,
    CWeaponTec9 = 238,
    CWeaponTMP = 239,
    CWeaponUMP45 = 240,
    CWeaponUSP = 241,
    CWeaponXM1014 = 242,
    CWorld = 243,
    DustTrail = 244,
    MovieExplosion = 245,
    ParticleSmokeGrenade = 246,
    RocketTrail = 247,
    SmokeTrail = 248,
    SporeExplosion = 249,
    SporeTrail = 250
};
enum class CSGOClassID
{
    CAK47 = 1,
    CBaseAnimating = 2,
    CBaseAnimatingOverlay = 3,
    CBaseAttributableItem = 4,
    CBaseButton = 5,
    CBaseCombatCharacter = 6,
    CBaseCombatWeapon = 7,
    CBaseCSGrenade = 8,
    CBaseCSGrenadeProjectile = 9,
    CBaseDoor = 10,
    CBaseEntity = 11,
    CBaseFlex = 12,
    CBaseGrenade = 13,
    CBaseParticleCBaseEntity = 14,
    CBasePlayer = 15,
    CBasePropDoor = 16,
    CBaseTeamObjectiveResource = 17,
    CBaseTempCBaseEntity = 18,
    CBaseToggle = 19,
    CBaseTrigger = 20,
    CBaseViewModel = 21,
    CBaseVPhysicsTrigger = 22,
    CBaseWeaponWorldModel = 23,
    CBeam = 24,
    CBeamSpotlight = 25,
    CBoneFollower = 26,
    CBreakableProp = 27,
    CBreakableSurface = 28,
    CC4 = 29,
    CCascadeLight = 30,
    CChicken = 31,
    CColorCorrection = 32,
    CColorCorrectionVolume = 33,
    CCSGameRulesProxy = 34,
    CCSPlayer = 35,
    CCSPlayerResource = 36,
    CCSRagdoll = 37,
    CCSTeam = 38,
    CDEagle = 39,
    CDecoyGrenade = 40,
    CDecoyProjectile = 41,
    CDynamicLight = 42,
    CDynamicProp = 43,
    CEconCBaseEntity = 44,
    CEconWearable = 45,
    CEmbers = 46,
    CCBaseEntityDissolve = 47,
    CCBaseEntityFlame = 48,
    CCBaseEntityFreezing = 49,
    CCBaseEntityParticleTrail = 50,
    CEnvAmbientLight = 51,
    CEnvDetailController = 52,
    CEnvDOFController = 53,
    CEnvParticleScript = 54,
    CEnvProjectedTexture = 55,
    CEnvQuadraticBeam = 56,
    CEnvScreenEffect = 57,
    CEnvScreenOverlay = 58,
    CEnvTonemapController = 59,
    CEnvWind = 60,
    CFEPlayerDecal = 61,
    CFireCrackerBlast = 62,
    CFireSmoke = 63,
    CFireTrail = 64,
    CFish = 65,
    CFlashbang = 66,
    CFogController = 67,
    CFootstepControl = 68,
    CFunc_Dust = 69,
    CFunc_LOD = 70,
    CFuncAreaPortalWindow = 71,
    CFuncBrush = 72,
    CFuncConveyor = 73,
    CFuncLadder = 74,
    CFuncMonitor = 75,
    CFuncMoveLinear = 76,
    CFuncOccluder = 77,
    CFuncReflectiveGlass = 78,
    CFuncRotating = 79,
    CFuncSmokeVolume = 80,
    CFuncTrackTrain = 81,
    CGameRulesProxy = 82,
    CHandleTest = 83,
    CHEGrenade = 84,
    CHostage = 85,
    CHostageCarriableProp = 86,
    CIncendiaryGrenade = 87,
    CInferno = 88,
    CInfoLadderDismount = 89,
    CInfoOverlayAccessor = 90,
    CItem_Healthshot = 91,
    CItemDogtags = 92,
    CKnife = 93,
    CKnifeGG = 94,
    CLightGlow = 95,
    CMaterialModifyControl = 96,
    CMolotovGrenade = 97,
    CMolotovProjectile = 98,
    CMovieDisplay = 99,
    CParticleFire = 100,
    CParticlePerformanceMonitor = 101,
    CParticleSystem = 102,
    CPhysBox = 103,
    CPhysBoxMultiplayer = 104,
    CPhysicsProp = 105,
    CPhysicsPropMultiplayer = 106,
    CPhysMagnet = 107,
    CPlantedC4 = 108,
    CPlasma = 109,
    CPlayerResource = 110,
    CPointCamera = 111,
    CPointCommentaryNode = 112,
    CPointWorldText = 113,
    CPoseController = 114,
    CPostProcessController = 115,
    CPrecipitation = 116,
    CPrecipitationBlocker = 117,
    CPredictedViewModel = 118,
    CProp_Hallucination = 119,
    CPropDoorRotating = 120,
    CPropJeep = 121,
    CPropVehicleDriveable = 122,
    CRagdollManager = 123,
    CRagdollProp = 124,
    CRagdollPropAttached = 125,
    CRopeKeyframe = 126,
    CSCAR17 = 127,
    CSceneCBaseEntity = 128,
    CSensorGrenade = 129,
    CSensorGrenadeProjectile = 130,
    CShadowControl = 131,
    CSlideshowDisplay = 132,
    CSmokeGrenade = 133,
    CSmokeGrenadeProjectile = 134,
    CSmokeStack = 135,
    CSpatialCBaseEntity = 136,
    CSpotlightEnd = 137,
    CSprite = 138,
    CSpriteOriented = 139,
    CSpriteTrail = 140,
    CStatueProp = 141,
    CSteamJet = 142,
    CSun = 143,
    CSunlightShadowControl = 144,
    CTeam = 145,
    CTeamplayRoundBasedRulesProxy = 146,
    CTEArmorRicochet = 147,
    CTEBaseBeam = 148,
    CTEBeamEntPoint = 149,
    CTEBeamEnts = 150,
    CTEBeamFollow = 151,
    CTEBeamLaser = 152,
    CTEBeamPoints = 153,
    CTEBeamRing = 154,
    CTEBeamRingPoint = 155,
    CTEBeamSpline = 156,
    CTEBloodSprite = 157,
    CTEBloodStream = 158,
    CTEBreakModel = 159,
    CTEBSPDecal = 160,
    CTEBubbles = 161,
    CTEBubbleTrail = 162,
    CTEClientProjectile = 163,
    CTEDecal = 164,
    CTEDust = 165,
    CTEDynamicLight = 166,
    CTEEffectDispatch = 167,
    CTEEnergySplash = 168,
    CTEExplosion = 169,
    CTEFireBullets = 170,
    CTEFizz = 171,
    CTEFootprintDecal = 172,
    CTEFoundryHelpers = 173,
    CTEGaussExplosion = 174,
    CTEGlowSprite = 175,
    CTEImpact = 176,
    CTEKillPlayerAttachments = 177,
    CTELargeFunnel = 178,
    CTEMetalSparks = 179,
    CTEMuzzleFlash = 180,
    CTEParticleSystem = 181,
    CTEPhysicsProp = 182,
    CTEPlantBomb = 183,
    CTEPlayerAnimEvent = 184,
    CTEPlayerDecal = 185,
    CTEProjectedDecal = 186,
    CTERadioIcon = 187,
    CTEShatterSurface = 188,
    CTEShowLine = 189,
    CTesla = 190,
    CTESmoke = 191,
    CTESparks = 192,
    CTESprite = 193,
    CTESpriteSpray = 194,
    CTest_ProxyToggle_Networkable = 194,
    CTestTraceline = 196,
    CTEWorldDecal = 197,
    CTriggerPlayerMovement = 198,
    CTriggerSoundOperator = 199,
    CVGuiScreen = 200,
    CVoteController = 201,
    CWaterBullet = 202,
    CWaterLODControl = 203,
    CWeaponAug = 204,
    CWeaponAWP = 205,
    CWeaponBaseItem = 206,
    CWeaponBizon = 207,
    CWeaponCSBase = 208,
    CWeaponCSBaseGun = 209,
    CWeaponCycler = 210,
    CWeaponElite = 211,
    CWeaponFamas = 212,
    CWeaponFiveSeven = 213,
    CWeaponG3SG1 = 214,
    CWeaponGalil = 215,
    CWeaponGalilAR = 216,
    CWeaponGlock = 217,
    CWeaponHKP2000 = 218,
    CWeaponM249 = 219,
    CWeaponM3 = 220,
    CWeaponM4A1 = 221,
    CWeaponMAC10 = 222,
    CWeaponMag7 = 223,
    CWeaponMP5Navy = 224,
    CWeaponMP7 = 225,
    CWeaponMP9 = 226,
    CWeaponNegev = 227,
    CWeaponNOVA = 228,
    CWeaponP228 = 229,
    CWeaponP250 = 230,
    CWeaponP90 = 231,
    CWeaponSawedoff = 232,
    CWeaponSCAR20 = 233,
    CWeaponScout = 234,
    CWeaponSG550 = 235,
    CWeaponSG552 = 236,
    CWeaponSG556 = 237,
    CWeaponSSG08 = 238,
    CWeaponTaser = 239,
    CWeaponTec9 = 240,
    CWeaponTMP = 241,
    CWeaponUMP45 = 242,
    CWeaponUSP = 243,
    CWeaponXM1014 = 244,
    CWorld = 245,
    DustTrail = 246,
    MovieExplosion = 247,
    ParticleSmokeGrenade = 248,
    RocketTrail = 249,
    SmokeTrail = 250,
    SporeExplosion = 251,
    SporeTrail = 252
};

enum ItemSchemaIndex
{
    ITEM_NONE = 0,
    WEAPON_DEAGLE = 1,
    WEAPON_DUALS = 2,
    WEAPON_FIVE7 = 3,
    WEAPON_GLOCK = 4,
    WEAPON_AK47 = 7,
    WEAPON_AUG = 8,
    WEAPON_AWP = 9,
    WEAPON_FAMAS = 10,
    WEAPON_G3SG1 = 11,
    WEAPON_GALIL = 13,
    WEAPON_M249 = 14,
    WEAPON_M4A1 = 16,
    WEAPON_MAC10 = 17,
    WEAPON_P90 = 19,
    WEAPON_UMP45 = 24,
    WEAPON_XM1014 = 25,
    WEAPON_BIZON = 26,
    WEAPON_MAG7 = 27,
    WEAPON_NEGEV = 28,
    WEAPON_SAWEDOFF = 29,
    WEAPON_TEC9 = 30,
    WEAPON_TASER = 31,
    WEAPON_P2000 = 32,
    WEAPON_MP7 = 33,
    WEAPON_MP9 = 34,
    WEAPON_NOVA = 35,
    WEAPON_P250 = 36,
    WEAPON_SCAR20 = 38,
    WEAPON_SG553 = 39,
    WEAPON_SCOUT = 40,
    WEAPON_KNIFE = 42,
    WEAPON_KNIFE_T = 59,
    WEAPON_FLASH = 43,
    WEAPON_HE = 44,
    WEAPON_SMOKE = 45,
    WEAPON_MOLOTOV = 46,
    WEAPON_DECOY = 47,
    WEAPON_INC = 48,
    WEAPON_C4 = 49,
    WEAPON_KNIFE_CT = 59,
    WEAPON_M4A1S = 60,
    WEAPON_USPS = 61,
    WEAPON_CZ75 = 63,
    WEAPON_REVOLVER = 64,
    WEAPON_MAX = 65,
    WEAPON_KNIFE_BAYONET = 500,
    WEAPON_KNIFE_FLIP = 505,
    WEAPON_KNIFE_GUT = 506,
    WEAPON_KNIFE_KARAMBIT = 507,
    WEAPON_KNIFE_M9_BAYONET = 508,
    WEAPON_KNIFE_TACTICAL = 509,
    WEAPON_KNIFE_FALCHION = 512,
    WEAPON_KNIFE_SURVIVAL_BOWIE = 514,
    WEAPON_KNIFE_BUTTERFLY = 515,
    WEAPON_KNIFE_PUSH = 516,
    GLOVE_STUDDED_BLOODHOUND = 5027,
    GLOVE_T_SIDE = 5028,
    GLOVE_CT_SIDE = 5029,
    GLOVE_SPORTY = 5030,
    GLOVE_SLICK = 5031,
    GLOVE_LEATHER_WRAP = 5032,
    GLOVE_MOTORCYCLE = 5033,
    GLOVE_SPECIALIST = 5034
};

enum class CSGOHitboxID
{
    HITBOX_HEAD,
    HITBOX_NECK,
    HITBOX_PELVIS,
    HITBOX_BELLY,
    HITBOX_THORAX,
    HITBOX_LOWER_CHEST,
    HITBOX_UPPER_CHEST,
    HITBOX_RIGHT_THIGH,
    HITBOX_LEFT_THIGH,
    HITBOX_RIGHT_CALF,
    HITBOX_LEFT_CALF,
    HITBOX_RIGHT_FOOT,
    HITBOX_LEFT_FOOT,
    HITBOX_RIGHT_HAND,
    HITBOX_LEFT_HAND,
    HITBOX_RIGHT_UPPER_ARM,
    HITBOX_RIGHT_FOREARM,
    HITBOX_LEFT_UPPER_ARM,
    HITBOX_LEFT_FOREARM,
    HITBOX_MAX
};
#define	HITGROUP_GENERIC	0
#define	HITGROUP_HEAD		1
#define	HITGROUP_CHEST		2
#define	HITGROUP_STOMACH	3
#define HITGROUP_LEFTARM	4	
#define HITGROUP_RIGHTARM	5
#define HITGROUP_LEFTLEG	6
#define HITGROUP_RIGHTLEG	7
#define HITGROUP_PELVIS	    8
#define HITGROUP_RIGHT_FOOT	9
#define HITGROUP_LEFT_FOOT	10
#define HITGROUP_GEAR		11			// alerts NPC, but doesn't do damage or bleed (1/100th damage)