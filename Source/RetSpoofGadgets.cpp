#include "RetSpoofGadgets.h"

#include "Memory.h"

RetSpoofGadgets::RetSpoofGadgets()
#ifdef _WIN32
    : jmpEbxInClient{ findPattern(CLIENT_DLL, "\x1B\xFF\x23\xF8\xF6\x87") + 1 },
      engine{ findPattern(ENGINE_DLL, "\xFF\x23") }
#endif
{
}
