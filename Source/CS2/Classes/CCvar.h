#pragma once

#include "ConVar.h"
#include "CUtlLinkedList.h"

namespace cs2
{

struct CCvar {
    using ConVarList = CUtlLinkedList<ConVar*>;
};

}
