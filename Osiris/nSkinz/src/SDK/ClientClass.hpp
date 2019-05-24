#pragma once
#include "declarations.hpp"

namespace sdk
{
	class ClientClass
	{
	public:
		CreateClientClassFn m_pCreateFn;
		CreateEventFn m_pCreateEventFn;
		const char* m_pNetworkName;
		RecvTable* m_pRecvTable;
		ClientClass* m_pNext;
		int m_ClassID;
	};

}
