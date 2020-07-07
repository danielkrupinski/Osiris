#pragma once
#include "../Hooks.h"
#include "Entity.h"

struct Container {
	Container() : m_hooked(false) { }

	bool m_hooked;
};

class HookListener
{
public:
	std::array< Container, 65 > m_players;
	bool init();
	void remove();
};

extern HookListener hookListener;