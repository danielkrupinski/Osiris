#pragma once

namespace sdk
{
	class IVModelInfoClient
	{
	public:
		virtual					~IVModelInfoClient() {};
		virtual const model_t*	GetModel(int modelindex) const = 0;
		virtual int				GetModelIndex(const char* name) const = 0;
		virtual const char*		GetModelName(const model_t* model) const = 0;
	};
}