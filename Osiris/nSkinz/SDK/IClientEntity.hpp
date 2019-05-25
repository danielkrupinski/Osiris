#pragma once

namespace sdk
{
	class IHandleEntity
	{
	public:
		virtual ~IHandleEntity() {}
		virtual void SetRefEHandle(const CBaseHandle& handle) = 0;
		virtual const CBaseHandle& GetRefEHandle() const = 0;
	};

	class IClientUnknown : public IHandleEntity
	{
	public:
		virtual ICollideable*			GetCollideable() = 0;
		virtual IClientNetworkable*		GetClientNetworkable() = 0;
		virtual IClientRenderable*		GetClientRenderable() = 0;
		virtual IClientEntity*			GetIClientEntity() = 0;
		virtual C_BaseEntity*			GetBaseEntity() = 0;
		virtual IClientThinkable*		GetClientThinkable() = 0;
		//virtual IClientModelRenderable*	GetClientModelRenderable() = 0;
		virtual IClientAlphaProperty*	GetClientAlphaProperty() = 0;
	};

	class IClientThinkable
	{
	public:
		virtual ~IClientThinkable() {};
	};

	class IClientRenderable
	{
	public:
		virtual ~IClientRenderable() {};
	};

	class IClientNetworkable
	{
	public:
		virtual IClientUnknown*	GetIClientUnknown() = 0;
		virtual void			Release() = 0;
		virtual ClientClass*	GetClientClass() = 0;
		virtual void			NotifyShouldTransmit(int state) = 0;
		virtual void			OnPreDataChanged(int updateType) = 0;
		virtual void			OnDataChanged(int updateType) = 0;
		virtual void			PreDataUpdate(int updateType) = 0;
		virtual void			PostDataUpdate(int updateType) = 0;
		virtual void			OnDataUnchangedInPVS() = 0;
		virtual bool			IsDormant() = 0;
		virtual int				EntIndex() const = 0;
		virtual void			ReceiveMessage(int classID, bf_read& msg) = 0;
		virtual void*			GetDataTableBasePtr() = 0;
		virtual void			SetDestroyedOnRecreateEntities() = 0;
	};

	class IClientEntity : public IClientUnknown, public IClientRenderable, public IClientNetworkable, public IClientThinkable
	{
	public:
		virtual ~IClientEntity() {};
	};
}