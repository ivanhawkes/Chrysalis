#include "StdAfx.h"

#include "ActionUnlock.h"
#include <CryDynamicResponseSystem/IDynamicResponseSystem.h>


DRS::IResponseActionInstanceUniquePtr CActionUnlock::Execute(DRS::IResponseInstance* pResponseInstance)
{
	IEntity* pEntity = pResponseInstance->GetCurrentActor()->GetLinkedEntity();
	if (pEntity)
	{
		auto pContextVariables = pResponseInstance->GetContextVariables();

		// DO SOMETHING HERE e.g. populate variables and execute action.

		return DRS::IResponseActionInstanceUniquePtr (new CActionUnlockInstance());
	}

	return nullptr;
}


string CActionUnlock::GetVerboseInfo() const
{
	return string("'") + m_targetName + "'";
}


void CActionUnlock::Serialize(Serialization::IArchive& ar)
{
	ar(m_targetName, "TargetName", "^ TargetName");
}


CActionUnlockInstance::CActionUnlockInstance()
{
}


CActionUnlockInstance::~CActionUnlockInstance()
{
}


DRS::IResponseActionInstance::eCurrentState CActionUnlockInstance::Update()
{
	return DRS::IResponseActionInstance::CS_FINISHED;
}


void CActionUnlockInstance::Cancel()
{
}
