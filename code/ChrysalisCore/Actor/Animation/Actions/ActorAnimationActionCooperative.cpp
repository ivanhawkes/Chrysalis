#include "StdAfx.h"

#include "ActorAnimationActionCooperative.h"
#include <Actor/ActorComponent.h>


namespace Chrysalis
{
CActorAnimationActionCooperative::CActorAnimationActionCooperative(CActorComponent &sourceActor, EntityId targetEntityId, FragmentID fragmentID, TagState tagState, TagID targetTagID, const IAnimationDatabase* pTargetOptionalDatabase)
	: CAnimationAction(EActorActionPriority::eAAP_ActionUrgent, fragmentID, tagState),
	m_sourceActor(sourceActor),
	m_targetEntityId(targetEntityId),
	m_targetTagID(targetTagID),
	m_pTargetOptionalDatabase(pTargetOptionalDatabase)
{
	if (gEnv->bMultiplayer)
	{
		gEnv->pEntitySystem->AddEntityEventListener(m_targetEntityId, ENTITY_EVENT_DONE, this);
	}

	// Try and get a pointer to an actor for the target, if it's possible. This opens up better code paths for animation.
	m_pTargetActor = CActorComponent::GetActor(m_targetEntityId);

	// Determine a suitable scope context.
	if (m_pTargetActor)
		m_scopeContextId = m_sourceActor.GetMannequinParams()->contextIDs.SlaveCharacter;
	else
		m_scopeContextId = m_sourceActor.GetMannequinParams()->contextIDs.SlaveObject;
}


CActorAnimationActionCooperative::~CActorAnimationActionCooperative()
{
	if (gEnv->bMultiplayer)
	{
		gEnv->pEntitySystem->RemoveEntityEventListener(m_targetEntityId, ENTITY_EVENT_DONE, this);
	}
}


void CActorAnimationActionCooperative::Install()
{
	CAnimationAction::Install();

	AddTargetToSlaveContext();
}


void CActorAnimationActionCooperative::Enter()
{
	CAnimationAction::Enter();

	QuatT targetPos(m_rootScope->GetEntity().GetPos(), m_rootScope->GetEntity().GetRotation());

	SetParam("TargetPos", targetPos);

	//if (auto pAnimChar = m_sourceActor.GetAnimatedCharacter())
	//{
	//	pAnimChar->SetMovementControlMethods(eMCM_Animation, eMCM_Animation);
	//	pAnimChar->RequestPhysicalColliderMode(eColliderMode_Disabled, eColliderModeLayer_Game, "CActorAnimationActionCooperative::Enter()");
	//}

	//if (m_pTargetActor)
	//{
	//	if (auto pAnimCharTarget = m_pTargetActor->GetAnimatedCharacter())
	//	{
	//		pAnimCharTarget->SetMovementControlMethods(eMCM_Animation, eMCM_Animation);
	//		pAnimCharTarget->RequestPhysicalColliderMode(eColliderMode_Disabled, eColliderModeLayer_Game, "CActorAnimationActionCooperative::Enter()");
	//	}
	//}
}


void CActorAnimationActionCooperative::Exit()
{
	CAnimationAction::Exit();

	SendStateEventCoopAnim();

	//if (auto pAnimChar = m_sourceActor.GetAnimatedCharacter())
	//{
	//	pAnimChar->SetMovementControlMethods(eMCM_Entity, eMCM_Entity);
	//	pAnimChar->ForceRefreshPhysicalColliderMode();
	//	pAnimChar->RequestPhysicalColliderMode(eColliderMode_Undefined, eColliderModeLayer_Game, "CActorAnimationActionCooperative::Exit()");
	//}

	//if (m_pTargetActor)
	//{
	//	if (auto pAnimCharTarget = m_pTargetActor->GetEntityId() ? m_pTargetActor->GetAnimatedCharacter() : nullptr)
	//	{
	//		pAnimCharTarget->SetMovementControlMethods(eMCM_Entity, eMCM_Entity);
	//		pAnimCharTarget->ForceRefreshPhysicalColliderMode();
	//		pAnimCharTarget->RequestPhysicalColliderMode(eColliderMode_Undefined, eColliderModeLayer_Game, "CActorAnimationActionCooperative::Exit()");
	//	}
	//}

	RemoveTargetFromSlaveContext();
}


void CActorAnimationActionCooperative::OnEntityEvent(IEntity *pEntity, SEntityEvent &event)
{
	if (!pEntity)
		return;

	const EntityId entityId = pEntity->GetId();
	if (entityId == m_targetEntityId)
	{
		// Cleaning the slave context currently /immediately/ flush scopes using
		// it, which means WE will get flushed too. In that case we will get a
		// Release() call. By adding a reference to ourselves we prevent ourselves
		// from being deleted in flight.
		AddRef();

		if (m_eStatus == Pending)
		{
			// In the unlikely event we didn't even start yet, make sure to send the
			// event that tells the rest of the code we finished
			SendStateEventCoopAnim();

			ForceFinish();
		}
		else if (m_eStatus == Installed)
		{
			// Note: Cleaning the slave context will cause Exit() to be called on ourselves
			RemoveTargetFromSlaveContext();
		}

		// Release ourselves after we cleaned up (this might cause ourselves to be deleted)
		Release();
	}
}


void CActorAnimationActionCooperative::AddTargetToSlaveContext()
{
	if (const auto pSourceActionController = m_sourceActor.GetActionController())
	{		
		IActionController* pTargetActionController { nullptr };

		// Check which way we will enslave the target actor.
		if ((m_pTargetActor) && (pTargetActionController = m_pTargetActor->GetActionController()))
		{
			// There is an action controller on the target actor, so we directly slave that to our action controller.
			pSourceActionController->SetSlaveController(*pTargetActionController,
				m_scopeContextId, true, m_pTargetOptionalDatabase);

			// Set the target tag.
			pTargetActionController->GetContext().state.Set(m_targetTagID, true);
		}
		else
		{
			// No action controller, so instead we set the scope content to our slave character context.			
			if (auto pTargetEntity = gEnv->pEntitySystem->GetEntity(m_targetEntityId))
			{
				pSourceActionController->SetScopeContext(m_scopeContextId, *pTargetEntity,
					pTargetEntity->GetCharacter(0), m_pTargetOptionalDatabase);
			}
		}
	}
}


void CActorAnimationActionCooperative::RemoveTargetFromSlaveContext()
{
	IActionController* pTargetActionController { nullptr };

	if ((m_pTargetActor) && (pTargetActionController = m_pTargetActor->GetActionController()))
	{
		// Remove the slave scope context.
		if (const auto pSourceActionController = m_sourceActor.GetActionController())
		{
			pSourceActionController->SetSlaveController(*pTargetActionController, m_scopeContextId, false, m_pTargetOptionalDatabase);

			// Remove the target tag.
			pTargetActionController->GetContext().state.Set(m_targetTagID, false);
		}
	}
	else
	{
		// Remove the slave scope context.
		m_rootScope->GetActionController().ClearScopeContext(m_scopeContextId);
	}
}


void CActorAnimationActionCooperative::SendStateEventCoopAnim()
{
	// TODO: We have moved the state machine for movement up to derived classes, like CCharacterComponent. I need to investigate this code
	// to determine if it's needed, and if so, add a method(s) to IActor to fire off events like these.
	//m_sourceActor.StateMachineHandleEventMovement(SStateEventCoopAnim(m_targetEntityId));
}
}