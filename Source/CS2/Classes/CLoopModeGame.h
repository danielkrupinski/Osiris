#pragma once

namespace cs2
{

struct CLoopModeGame {
    using getWorldSession = void* (*)(CLoopModeGame* thisptr);
};

}
