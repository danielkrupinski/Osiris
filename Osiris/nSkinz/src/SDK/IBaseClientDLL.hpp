#pragma once

namespace sdk
{
	enum ClientFrameStage_t : int
	{
		FRAME_UNDEFINED = -1,
		FRAME_START,
		FRAME_NET_UPDATE_START,
		FRAME_NET_UPDATE_POSTDATAUPDATE_START,
		FRAME_NET_UPDATE_POSTDATAUPDATE_END,
		FRAME_NET_UPDATE_END,
		FRAME_RENDER_START,
		FRAME_RENDER_END
	};

	class IBaseClientDLL
	{
	public:
		ClientClass* GetAllClasses()
		{
			return get_vfunc<ClientClass*(__thiscall *)(IBaseClientDLL*)>(this, 8)(this);
		}
	};

}