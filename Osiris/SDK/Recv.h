#pragma once

#include "Vector.h"

struct recvProxyData {
    int pad;
    union {
        float floatValue;
        long intValue;
        char* stringValue;
        void* data;
        Vector vector;
        int64_t  int64Value;
    };
};

using recvProxy = std::add_pointer_t<void(recvProxyData*, void*, void*)>;

struct RecvProp {
    char* name;
    int type;
    int flags;
    int stringBufferSize;
    int insideArray;
    const void* extraData;
    RecvProp* arrayProp;
    void* arrayLengthProxy;
    recvProxy proxy;
    void* dataTableProxy;
    struct RecvTable* dataTable;
    int offset;
    int elementStride;
    int elementCount;
    const char* parentArrayPropName;
};

struct RecvTable {
    RecvProp* props;
    int propCount;
    void* decoder;
    char* netTableName;
    bool isInitialized;
    bool isInMainList;
};
