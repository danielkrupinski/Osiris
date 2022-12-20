#pragma once

#include <cstdint>
#include <type_traits>

#include <Platform/Macros/CallingConventions.h>
#include "Vector.h"

namespace csgo
{

struct RecvProp;

struct recvProxyData {
    const RecvProp* recvProp;
    union {
        float _float;
        long _int;
        char* _string;
        void* data;
        Vector vector;
        int64_t int64;
    } value;
};

using RecvProxy = void (CDECL_CONV*)(recvProxyData* data, void* outStruct, void* out);

struct RecvProp {
    char* name;
    int type;
    int flags;
    int stringBufferSize;
    int insideArray;
    const void* extraData;
    RecvProp* arrayProp;
    void* arrayLengthProxy;
    RecvProxy proxy;
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

}
