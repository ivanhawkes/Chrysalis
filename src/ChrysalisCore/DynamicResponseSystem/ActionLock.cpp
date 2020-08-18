#include "StdAfx.h"

#include "ActionLock.h"
#include <CryDynamicResponseSystem/IDynamicResponseSystem.h>


namespace Chrysalis
{
DRS::IResponseActionInstanceUniquePtr CActionLock::Execute(DRS::IResponseInstance* pResponseInstance)
{
	IEntity* pEntity = pResponseInstance->GetCurrentActor()->GetLinkedEntity();
	if (pEntity)
	{
		auto pContextVariables = pResponseInstance->GetContextVariables();

		// DO SOMETHING HERE e.g. populate variables and execute action.

		return DRS::IResponseActionInstanceUniquePtr(new CActionLockInstance());
	}

	return nullptr;
}


string CActionLock::GetVerboseInfo() const
{
	return string("'") + m_targetName + "'";
}


void CActionLock::Serialize(Serialization::IArchive& ar)
{
	ar(m_targetName, "TargetName", "^ TargetName");
}


CActionLockInstance::CActionLockInstance()
{
}


CActionLockInstance::~CActionLockInstance()
{
}


DRS::IResponseActionInstance::eCurrentState CActionLockInstance::Update()
{
	return DRS::IResponseActionInstance::CS_FINISHED;
}


void CActionLockInstance::Cancel()
{
}
}