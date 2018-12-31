#pragma once

struct RecvProp {
    char* name;
    int type;
    int flags;
    int stringBufferSize;
    int m_bInsideArray;
    const void* m_pExtraData;
    RecvProp* m_pArrayProp;
    void* arrayLengthProxy;
    void* proxyFn;
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
