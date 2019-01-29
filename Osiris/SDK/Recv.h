#pragma once

#include "Vector.h"

struct recvProxyData {
    int pad;
    union {
        float floatValue;
        long intValue;
        char* stringValue;
        void* data;
        float vector[3];
        int64_t  int64Value;
    };
};

typedef void(*recvProxy)(const recvProxyData* pData, void* pStruct, void* pOut);

struct RecvProp {
    char* name;
    int type;
    int flags;
    int stringBufferSize;
    int m_bInsideArray;
    const void* m_pExtraData;
    RecvProp* m_pArrayProp;
    void* arrayLengthProxy;
    recvProxy proxy;
    void* dataTableProxyFn;
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
    char* m_pNetTableName;
    bool isInitialized;
    bool isInMainList;
};
