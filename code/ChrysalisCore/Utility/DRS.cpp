#include <StdAfx.h>

#include "DRS.h"


namespace DRSUtility
{
	// Helper functions to make getting values from context variables easier.

	int GetValueOrDefault(const DRS::IVariableCollectionSharedPtr pContextVariables, const CHashedString& name, const int default)
	{
		auto variable = pContextVariables->GetVariable(name);
		return variable ? variable->GetValueAsInt() : default;
	}


	float GetValueOrDefault(const DRS::IVariableCollectionSharedPtr pContextVariables, const CHashedString& name, const float default)
	{
		auto variable = pContextVariables->GetVariable(name);
		return variable ? variable->GetValueAsFloat() : default;
	}


	CHashedString GetValueOrDefault(const DRS::IVariableCollectionSharedPtr pContextVariables, const CHashedString& name, const CHashedString default)
	{
		auto variable = pContextVariables->GetVariable(name);
		return variable ? variable->GetValueAsHashedString() : default;
	}


	bool GetValueOrDefault(const DRS::IVariableCollectionSharedPtr pContextVariables, const CHashedString& name, const bool default)
	{
		auto variable = pContextVariables->GetVariable(name);
		return variable ? variable->GetValueAsBool() : default;
	}
}