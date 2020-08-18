#pragma once

#include <CryDynamicResponseSystem/IDynamicResponseSystem.h>


namespace Chrysalis
{
namespace DRSUtility
{
// Helper functions to make getting values from context variables easier.
int GetValueOrDefault(const DRS::IVariableCollectionSharedPtr pContextVariables, const CHashedString& name, const int defaultValue);
float GetValueOrDefault(const DRS::IVariableCollectionSharedPtr pContextVariables, const CHashedString& name, const float defaultValue);
CHashedString GetValueOrDefault(const DRS::IVariableCollectionSharedPtr pContextVariables, const CHashedString& name, const CHashedString defaultValue);
bool GetValueOrDefault(const DRS::IVariableCollectionSharedPtr pContextVariables, const CHashedString& name, const bool defaultValue);
}
}