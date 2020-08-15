#pragma once
#include "Hooks.h"
#include "SDK/Entity.h"

struct Container {
	Container() : m_hooked(false) { }

	Hooks::Vmt m_vmt;
	bool m_hooked;
};

class HookListener
{
public:
	std::array< Container, 65 > m_players;
	void hook_entity(Entity* ent);
	bool init();
	void remove();
};

extern HookListener hookListener; #pragma once
