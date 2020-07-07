#include <deque>
#include "Listener.h"
#include "ModelInfo.h"
#include "../Hacks/Backtrack.h"

typedef float quaternion_t[4];

HookListener hookListener;



bool HookListener::init()
{
	if (interfaces->engine->isInGame())
	{
		for (auto i = 1; i <= interfaces->engine->getMaxClients(); i++)
		{
			auto entity = interfaces->entityList->getEntity(i);

			if (!entity)
				continue;

		}
	}
	return true;
}

void HookListener::remove()
{
	for (auto i = 1; i <= 65; i++)
	{
		if (m_players[i].m_hooked)
		{
			m_players[i].m_hooked = false;
		}
	}
}