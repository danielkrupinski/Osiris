#pragma once

class HandleEntity {
public:
    virtual ~HandleEntity() {}
    virtual void SetRefEHandle(const long& handle) = 0;
    virtual const long& GetRefEHandle() const = 0;
};
