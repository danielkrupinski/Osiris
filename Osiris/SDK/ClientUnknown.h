#pragma once

#include "ClientNetworkable.h"
#include "HandleEntity.h"

class BaseEntity;

class ClientUnknown : public HandleEntity {
public:
    virtual void* /* ICollideable*  */            GetCollideable() = 0;
    virtual ClientNetworkable*        GetClientNetworkable() = 0;
    virtual void* /* IClientRenderable* */         GetClientRenderable() = 0;
    virtual void* /* IClientEntity* */            GetIClientEntity() = 0;
    virtual BaseEntity*                           GetBaseEntity() = 0;
    virtual void* /* IClientThinkable*  */        GetClientThinkable() = 0;
    //virtual IClientModelRenderable*  GetClientModelRenderable() = 0;
    virtual void* /* IClientAlphaProperty* */      GetClientAlphaProperty() = 0;
};
