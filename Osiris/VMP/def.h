#include "VMProtectSDK.h"

//#define ENABLE_VMP 

#ifdef ENABLE_VMP
#define VMP_VIRTUAL(e) VMProtectBeginVirtualization(e);
#define VMP_MUTATION(e) VMProtectBeginMutation(e);
#define VMP_ULTRA(e) VMProtectBeginUltra(e);
#define VMP_BEGIN(e) VMProtectBegin(e);
#define VMP_END VMProtectEnd();
#else
#define VMP_VIRTUAL(e)
#define VMP_MUTATION(e)
#define VMP_ULTRA(e)
#define VMP_BEGIN(e)
#define VMP_END 
#endif
