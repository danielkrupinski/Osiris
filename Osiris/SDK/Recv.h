#pragma once

struct recvProxyData {
    int pad;
    int value;
};

using recvProxy = void(*)(const recvProxyData* data, void* struct_, void* out);

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
