#pragma once

namespace sdk
{
	class IInputSystem
	{
	public:
		void EnableInput(bool bEnable)
		{
			return get_vfunc<void(__thiscall *)(IInputSystem*, bool)>(this, 11)(this, bEnable);
		}

		void*& get_window()
		{
			static uint32_t offset = 0;
			if (!offset)
				offset = *(uint32_t*)((*(char***)this)[10] + 5);
			return *(void**)((char*)this + offset);
		}
	};

}