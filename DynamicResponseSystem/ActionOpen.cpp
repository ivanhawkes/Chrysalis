#include "StdAfx.h"

#include "ActionOpen.h"
#include <CryDynamicResponseSystem/IDynamicResponseSystem.h>


DRS::IResponseActionInstanceUniquePtr CActionOpen::Execute(DRS::IResponseInstance* pResponseInstance)
{
	IEntity* pEntity = pResponseInstance->GetCurrentActor()->GetLinkedEntity();
	if (pEntity)
	{
		auto pContextVariables = pResponseInstance->GetContextVariables();

		// DO SOMETHING HERE e.g. populate variables and execute action.

		return DRS::IResponseActionInstanceUniquePtr (new CActionOpenInstance());
	}

	return nullptr;
}


string CActionOpen::GetVerboseInfo() const
{
	return string("'") + m_targetName + "'";
}


void CActionOpen::Serialize(Serialization::IArchive& ar)
{
	ar(m_targetName, "TargetName", "^ TargetName");
}


CActionOpenInstance::CActionOpenInstance()
{
}


CActionOpenInstance::~CActionOpenInstance()
{
}


DRS::IResponseActionInstance::eCurrentState CActionOpenInstance::Update()
{
	return DRS::IResponseActionInstance::CS_FINISHED;
}


void CActionOpenInstance::Cancel()
{
}
