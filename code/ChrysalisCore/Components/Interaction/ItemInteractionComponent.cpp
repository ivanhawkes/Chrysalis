#include <StdAfx.h>

#include "ItemInteractionComponent.h"
#include <Components/Interaction/EntityInteractionComponent.h>
#include "Components/Player/PlayerComponent.h"
#include <Components/Player/Input/PlayerInputComponent.h>
#include <Actor/Character/CharacterComponent.h>


namespace Chrysalis
{
void CItemInteractionComponent::Register(Schematyc::CEnvRegistrationScope& componentScope)
{
}


void CItemInteractionComponent::ReflectType(Schematyc::CTypeDesc<CItemInteractionComponent>& desc)
{
	desc.SetGUID(CItemInteractionComponent::IID());
	desc.SetEditorCategory("Interaction");
	desc.SetLabel("Item Interaction");
	desc.SetDescription("No description.");
	desc.SetIcon("icons:ObjectTypes/light.ico");
	desc.SetComponentFlags({ IEntityComponent::EFlags::Singleton });
}


// ***
// *** IEntityComponent
// ***


void CItemInteractionComponent::Initialize()
{
	// Add new verbs to the interactor.
	m_interactor = GetEntity()->GetOrCreateComponent<CEntityInteractionComponent>();
	if (m_interactor)
	{
		m_interactor->AddInteraction(std::make_shared<CInteractionItemInspect>(this));
		m_interactor->AddInteraction(std::make_shared<CInteractionItemPickup>(this));
		m_interactor->AddInteraction(std::make_shared<CInteractionItemDrop>(this, true, true));
		m_interactor->AddInteraction(std::make_shared<CInteractionItemToss>(this, true, true));
	}

	// Reset the entity.
	OnResetState();
}


void CItemInteractionComponent::ProcessEvent(SEntityEvent& event)
{
	switch (event.event)
	{
		case ENTITY_EVENT_UPDATE:
			Update();
			break;
	}
}


void CItemInteractionComponent::OnInteractionItemInspect(IActorComponent& actor)
{
	// We're already inspecting it, drop it instead.
	if (m_inspectionState == InspectionState::eInspecting)
	{
		OnInteractionItemDrop(actor);
		return;
	}

	CryLogAlways("OnInteractionItemInspect fired.");
	m_inspectionState = InspectionState::eInspecting;

	if (auto pActorComponent = CPlayerComponent::GetLocalActor())
	{
		const auto pEntity = GetEntity();
		m_initialPosition = pEntity->GetPos();
		m_initialRotation = pEntity->GetRotation();
		m_targetPosition = pActorComponent->GetEntity()->GetPos() + pActorComponent->GetLocalLeftHandPos();
		m_timeInAir = 0.0f;
		m_timeInAirRequired = (m_targetPosition - m_initialPosition).GetLength() / kJumpToPlayerSpeed;

		// #TODO: Use a helper method instead of setting directly.
		if (auto pPlayer = CPlayerComponent::GetLocalPlayer())
		{
			pPlayer->GetinteractionState().SetInteractionMode(CPlayerComponent::SInteractionState::EInteractionMode::eHandlingEntity);
		}
	}
}


void CItemInteractionComponent::OnInteractionItemPickup(IActorComponent& actor)
{
	// We're already inspecting it, drop it instead.
	if (m_inspectionState == InspectionState::ePickingUp)
	{
		OnInteractionItemDrop(actor);
		return;
	}

	CryLogAlways("OnInteractionItemPickup fired.");
	m_inspectionState = InspectionState::ePickingUp;

	if (auto pActorComponent = CPlayerComponent::GetLocalActor())
	{
		m_initialPosition = GetEntity()->GetPos();
		m_targetPosition = pActorComponent->GetEntity()->GetPos() + pActorComponent->GetLocalRightHandPos();
		m_timeInAir = 0.0f;
		m_timeInAirRequired = (m_targetPosition - m_initialPosition).GetLength() / kJumpToPlayerSpeed;

		// #TODO: Use a helper method instead of setting directly.
		if (auto pPlayer = CPlayerComponent::GetLocalPlayer())
		{
			pPlayer->GetinteractionState().SetInteractionMode(CPlayerComponent::SInteractionState::EInteractionMode::eHandlingEntity);
		}
	}
}


void CItemInteractionComponent::OnInteractionItemDrop(IActorComponent& actor)
{
	CryLogAlways("OnInteractionItemDrop fired.");
	m_inspectionState = InspectionState::eDroping;

	if (auto pActorComponent = CPlayerComponent::GetLocalActor())
	{
		const auto pEntity = GetEntity();

		pe_action_awake action;
		action.bAwake = true;

		// No impulse, we're just letting it drop.
		IPhysicalEntity* pPhysEntity = pEntity->GetPhysics();
		if (pPhysEntity)
			pPhysEntity->Action(&action);
	}

	if (auto pPlayer = CPlayerComponent::GetLocalPlayer())
	{
		pPlayer->GetinteractionState().SetInteractionMode(CPlayerComponent::SInteractionState::EInteractionMode::eNoMode);
	}
}


void CItemInteractionComponent::OnInteractionItemToss(IActorComponent& actor)
{
	CryLogAlways("OnInteractionItemToss fired.");
	m_inspectionState = InspectionState::eTossing;

	if (auto pActorComponent = CPlayerComponent::GetLocalActor())
	{
		const auto pEntity = GetEntity();
		Vec3 impulse = pActorComponent->GetEntity()->GetRotation() * FORWARD_DIRECTION * kTossNewtons;

		// A small impulse to toss it aside.
		pe_action_impulse action;
		action.impulse = impulse;
		action.point = pActorComponent->GetLocalRightHandPos();

		IPhysicalEntity* pPhysEntity = pEntity->GetPhysics();
		if (pPhysEntity)
			pPhysEntity->Action(&action);
	}

	if (auto pPlayer = CPlayerComponent::GetLocalPlayer())
	{
		pPlayer->GetinteractionState().SetInteractionMode(CPlayerComponent::SInteractionState::EInteractionMode::eNoMode);
	}
}


// ***
// *** CItemInteractionComponent
// ***


void CItemInteractionComponent::OnResetState()
{
}


void CItemInteractionComponent::Update()
{
	const float frameTime = gEnv->pTimer->GetFrameTime();

	switch (m_inspectionState)
	{
		case InspectionState::eInspecting:
			OnInspectingUpdate(frameTime);
			break;

		case InspectionState::ePickingUp:
			OnPickingUpUpdate(frameTime);
			break;

			//case InspectionState::eDroping:
			//	// #TODO: Start physics simulating again.
			//	break;

			//case InspectionState::eTossing:
			//	// #TODO: Start physics simulating again.
			//	break;

			//case InspectionState::eCancelled:
			//	// #TODO: Start physics simulating again.
			//	break;

			//case InspectionState::eNone:
			//	// #TODO: Start physics simulating again.
			//	break;
	}
}


void CItemInteractionComponent::OnInspectingUpdate(const float frameTime)
{
	const auto pEntity = GetEntity();

	// For now, just pull the object to a target location.
	m_timeInAir = min(m_timeInAirRequired, m_timeInAir + frameTime);

	if (auto pActorComponent = CPlayerComponent::GetLocalActor())
	{
		Vec3 delta = (m_targetPosition - m_initialPosition) * (1.0f - (m_timeInAir / m_timeInAirRequired));
		pEntity->SetPos(m_targetPosition - delta);

		// Allow them to rotate the item in their hands.
		if (auto pPlayerInput = CPlayerComponent::GetLocalPlayer()->GetPlayerInput())
		{
			// #TODO: The rotation flips the controls when the item is upside down. That feels weird. Is there a way to remove this
			// from the rotation?
			const auto inputRotation = Quat(Ang3(pPlayerInput->GetPitchDelta() * kInspectionRotationFactor, pPlayerInput->GetYawDelta() * kInspectionRotationFactor, 0.0f));
			const auto characterRotation = Quat(Ang3(0.0f, 0.0f, pActorComponent->GetEntity()->GetRotation().GetFwdZ())).GetNormalized();
			pEntity->SetRotation(pEntity->GetRotation() * inputRotation * characterRotation);
		}
	}
}


void CItemInteractionComponent::OnPickingUpUpdate(const float frameTime)
{
	const auto pEntity = GetEntity();

	// For now, just pull the object to a target location.
	m_timeInAir = min(m_timeInAirRequired, m_timeInAir + frameTime);

	if (auto pActorComponent = CPlayerComponent::GetLocalActor())
	{
		Vec3 delta = (m_targetPosition - m_initialPosition) * (1.0f - (m_timeInAir / m_timeInAirRequired));
		pEntity->SetPos(m_targetPosition - delta);
	}

	// Allow them to rotate the item in their hands.
	if (auto pPlayerInput = CPlayerComponent::GetLocalPlayer()->GetPlayerInput())
	{
		const auto rotation = Quat(Ang3(0.0f, 0.0f, pPlayerInput->GetYawDelta()));
		pEntity->SetRotation(pEntity->GetRotation() * rotation);
	}
}
}