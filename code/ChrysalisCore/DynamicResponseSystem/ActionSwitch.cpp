#include "StdAfx.h"

#include "ActionSwitch.h"
#include <CryDynamicResponseSystem/IDynamicResponseSystem.h>
#include "Components/Player/PlayerComponent.h"
#include "Actor/ActorComponent.h"
#include "Utility/DRS.h"
#include <Components/Interaction/EntityInteractionComponent.h>


namespace Chrysalis
{
DRS::IResponseActionInstanceUniquePtr CActionSwitch::Execute(DRS::IResponseInstance* pResponseInstance)
{
	auto pResponseActor = pResponseInstance->GetCurrentActor();
	auto pContextVariables = pResponseInstance->GetContextVariables();

	if (pResponseActor && pContextVariables)
	{
		IEntity* const pEntity = pResponseActor->GetLinkedEntity();

		if (pEntity)
		{
			// They may have sent us a different verb to the standard one.
			CHashedString verb = DRSUtility::GetValueOrDefault(pContextVariables, "Verb", CHashedString(""));

			// This allows us to select between being switched on and off.
			// #TODO: Put this into use and look into what else we can add.
			bool isSwitchOn = DRSUtility::GetValueOrDefault(pContextVariables, "IsSwitchedOn", false);

			if (auto pInteractor = pEntity->GetComponent<CEntityInteractionComponent>())
			{
				// Simple option is to play the verb.
				// #TODO: This should be a little more nuanced.
				auto pInteraction = pInteractor->GetInteraction(verb.GetText()).lock();
				if (pInteraction)
				{
					if (auto pActorComponent = CPlayerComponent::GetLocalActor())
						pInteraction->OnInteractionStart(*pActorComponent);
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
}