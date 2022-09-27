#include "RetSpoofGadgets.h"

#include "Memory.h"

RetSpoofGadgets::RetSpoofGadgets()
    : jmpEbxInClient{ findPattern(CLIENT_DLL, "\x1B\xFF\x23\xF8\xF6\x87") + 1 }
{
}
