#include "StdAfx.h"

#include "ActionSwitch.h"
#include <CryDynamicResponseSystem/IDynamicResponseSystem.h>
#include "Utility/DRS.h"
#include <Entities/Interaction/EntityInteractionComponent.h>


DRS::IResponseActionInstanceUniquePtr CActionSwitch::Execute(DRS::IResponseInstance* pResponseInstance)
{
	IEntity* pEntity = pResponseInstance->GetCurrentActor()->GetLinkedEntity();
	if (pEntity)
	{
		auto pResponseActor = pResponseInstance->GetCurrentActor();
		auto pContextVariables = pResponseInstance->GetContextVariables();

		if (pResponseActor && pContextVariables)
		{
			IEntity* const pEntity = pResponseActor->GetLinkedEntity();

			// The animation to play.
			CHashedString verb = DRSUtility::GetValueOrDefault(pContextVariables, "Verb", CHashedString(""));

			// Playback parameters.
			bool isSwitchOn = DRSUtility::GetValueOrDefault(pContextVariables, "IsSwitchedOn", false);

			if (auto pInteractor = pEntity->GetComponent<CEntityInteractionComponent>())
			{
				auto pInteraction = pInteractor->GetInteraction(verb.GetText())._Get();
				if (pInteraction)
				{
					pInteraction->OnInteractionStart();
				}
			}
		}

		return DRS::IResponseActionInstanceUniquePtr(new CActionSwitchInstance());
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


DRS::IResponseActionInstance::eCurrentState CActionSwitchInstance::Update()
{
	return DRS::IResponseActionInstance::CS_FINISHED;
}


void CActionSwitchInstance::Cancel()
{
}
