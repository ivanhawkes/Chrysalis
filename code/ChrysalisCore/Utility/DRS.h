#pragma once

#include <CryDynamicResponseSystem/IDynamicResponseSystem.h>


namespace Chrysalis
{
namespace DRSUtility
{
// Helper functions to make getting values from context variables easier.
int GetValueOrDefault(const DRS::IVariableCollectionSharedPtr pContextVariables, const CHashedString& name, const int default);
float GetValueOrDefault(const DRS::IVariableCollectionSharedPtr pContextVariables, const CHashedString& name, const float default);
CHashedString GetValueOrDefault(const DRS::IVariableCollectionSharedPtr pContextVariables, const CHashedString& name, const CHashedString default);
bool GetValueOrDefault(const DRS::IVariableCollectionSharedPtr pContextVariables, const CHashedString& name, const bool default);
}
}