#pragma once

#include "ClientClass.h"

class ClientNetworkable {
public:
    virtual void* /* IClientUnknown* */  GetIClientUnknown() = 0;
    virtual void             Release() = 0;
    virtual ClientClass*     GetClientClass() = 0;
    virtual void             NotifyShouldTransmit(int state) = 0;
    virtual void             OnPreDataChanged(int updateType) = 0;
    virtual void             OnDataChanged(int updateType) = 0;
    virtual void             PreDataUpdate(int updateType) = 0;
    virtual void             PostDataUpdate(int updateType) = 0;
    virtual void             __unkn(void) = 0;
    virtual bool             IsDormant(void) = 0;
    virtual int              EntIndex(void) const = 0;
    virtual void             ReceiveMessage(int classID, int&/* bf_read& */ msg) = 0;
    virtual void*            GetDataTableBasePtr() = 0;
    virtual void             SetDestroyedOnRecreateEntities(void) = 0;
};
