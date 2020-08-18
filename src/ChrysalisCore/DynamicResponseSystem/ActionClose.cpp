#include "StdAfx.h"

#include "ActionClose.h"
#include <CryDynamicResponseSystem/IDynamicResponseSystem.h>


namespace Chrysalis
{
DRS::IResponseActionInstanceUniquePtr CActionClose::Execute(DRS::IResponseInstance* pResponseInstance)
{
	IEntity* pEntity = pResponseInstance->GetCurrentActor()->GetLinkedEntity();
	if (pEntity)
	{
		auto pContextVariables = pResponseInstance->GetContextVariables();

		// DO SOMETHING HERE e.g. populate variables and execute action.

		return DRS::IResponseActionInstanceUniquePtr(new CActionCloseInstance());
	}

	return nullptr;
}


string CActionClose::GetVerboseInfo() const
{
	return string("'") + m_targetName + "'";
}


void CActionClose::Serialize(Serialization::IArchive& ar)
{
	ar(m_targetName, "TargetName", "^ TargetName");
}


CActionCloseInstance::CActionCloseInstance()
{
}


CActionCloseInstance::~CActionCloseInstance()
{
}


DRS::IResponseActionInstance::eCurrentState CActionCloseInstance::Update()
{
	return DRS::IResponseActionInstance::CS_FINISHED;
}


void CActionCloseInstance::Cancel()
{
}
}