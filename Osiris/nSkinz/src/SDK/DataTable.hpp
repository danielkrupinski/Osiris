#pragma once
#include "declarations.hpp"
#include <cstdint>

namespace sdk
{
	struct DVariant
	{
		union
		{
			float		m_Float;
			long		m_Int;
			char*		m_pString;
			void*		m_pData;
			Vector		m_Vector;
			int64_t		m_Int64;
		};

		int m_Type;
	};

	struct CRecvProxyData
	{
		const RecvProp*	m_pRecvProp;
		DVariant		m_Value;
		int				m_iElement;
		int				m_ObjectID;
	};

	enum SendPropType : int
	{
		DPT_Int = 0,
		DPT_Float,
		DPT_Vector,
		DPT_VectorXY,
		DPT_String,
		DPT_Array,
		DPT_DataTable,
		DPT_Int64,
		DPT_NUMSendPropTypes
	};

	class RecvProp
	{
	public:
		char*			m_pVarName;
		SendPropType	m_RecvType;
		int				m_Flags;
		int				m_StringBufferSize;
		bool			m_bInsideArray;
		const void*		m_pExtraData;
		RecvProp*		m_pArrayProp;
		void*			m_ArrayLengthProxy;
		RecvVarProxyFn	m_ProxyFn;
		void*			m_DataTableProxyFn;
		RecvTable*		m_pDataTable;
		int				m_Offset;
		int				m_ElementStride;
		int				m_nElements;
		const char*		m_pParentArrayPropName;
	};

	class RecvTable
	{
	public:
		RecvProp*	m_pProps;
		int			m_nProps;
		void*		m_pDecoder;
		char*		m_pNetTableName;
		bool		m_bInitialized;
		bool		m_bInMainList;
	};
}
