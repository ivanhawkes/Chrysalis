#include <StdAfx.h>

#include "DRS.h"


namespace Chrysalis
{
namespace DRSUtility
{
// Helper functions to make getting values from context variables easier.

int GetValueOrDefault(const DRS::IVariableCollectionSharedPtr pContextVariables, const CHashedString& name, const int defaultValue)
{
	auto variable = pContextVariables->GetVariable(name);
	return variable ? variable->GetValueAsInt() : defaultValue;
}


float GetValueOrDefault(const DRS::IVariableCollectionSharedPtr pContextVariables, const CHashedString& name, const float defaultValue)
{
	auto variable = pContextVariables->GetVariable(name);
	return variable ? variable->GetValueAsFloat() : defaultValue;
}


CHashedString GetValueOrDefault(const DRS::IVariableCollectionSharedPtr pContextVariables, const CHashedString& name, const CHashedString defaultValue)
{
	auto variable = pContextVariables->GetVariable(name);
	return variable ? variable->GetValueAsHashedString() : defaultValue;
}


bool GetValueOrDefault(const DRS::IVariableCollectionSharedPtr pContextVariables, const CHashedString& name, const bool defaultValue)
{
	auto variable = pContextVariables->GetVariable(name);
	return variable ? variable->GetValueAsBool() : defaultValue;
}
}
}