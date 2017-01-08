#include "StdAfx.h"

#include "ActionSwitch.h"
#include <CryDynamicResponseSystem/IDynamicResponseSystem.h>


DRS::IResponseActionInstanceUniquePtr CActionSwitch::Execute(DRS::IResponseInstance* pResponseInstance)
{
	IEntity* pEntity = pResponseInstance->GetCurrentActor()->GetLinkedEntity();
	if (pEntity)
	{
		auto pContextVariables = pResponseInstance->GetContextVariables();

		// DO SOMETHING HERE e.g. populate variables and execute action.

		return DRS::IResponseActionInstanceUniquePtr (new CActionSwitchInstance());
	}

	return nullptr;
}


string CActionSwitch::GetVerboseInfo() const
{
	return string("'") + m_targetName + "'";
}


void CActionSwitch::Serialize(Serialization::IArchive& ar)
{
	ar(m_targetName, "TargetName", "^ TargetName");
}


CActionSwitchInstance::CActionSwitchInstance()
{
}


CActionSwitchInstance::~CActionSwitchInstance()
{
}


DRS::IResponseActionInstance::eCurrentState CActionSwitchInstance::Update()
{
	return DRS::IResponseActionInstance::CS_FINISHED;
}


void CActionSwitchInstance::Cancel()
{
}
