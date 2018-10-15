#pragma once

#include "BaseHandle.h"

class HandleEntity {
public:
    virtual ~HandleEntity() {}
    virtual void SetRefEHandle(const BaseHandle &handle) = 0;
    virtual const BaseHandle& GetRefEHandle() const = 0;
};
