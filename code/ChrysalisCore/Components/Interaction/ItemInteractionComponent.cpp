#include <StdAfx.h>

#include "ItemInteractionComponent.h"
#include <Components/Interaction/EntityInteractionComponent.h>
#include <Player/Player.h>
#include <Player/Input/IPlayerInputComponent.h>
#include <Actor/Character/Character.h>


CRYREGISTER_CLASS(CItemInteractionComponent)


class CItemInteractionRegistrator : public IEntityRegistrator
{
	virtual void Register() override
	{
		// Register the class as a factory.
		RegisterEntityWithDefaultComponent<CItemInteractionComponent>("ItemInteractionComponent", "Interaction", "Item.bmp", true);

		// This should make the entity class invisible in the editor.
		auto cls = gEnv->pEntitySystem->GetClassRegistry()->FindClass("ItemInteractionComponent");
		cls->SetFlags(cls->GetFlags() | ECLF_INVISIBLE);
	}
};

CItemInteractionRegistrator g_ItemInteractionRegistrator;



// ***
// *** IEntityComponent
// ***


void CItemInteractionComponent::Initialize()
{
	// Required for 5.3 to call update.
	GetEntity()->Activate(true);

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
	CDesignerEntityComponent::ProcessEvent(event);

	switch (event.event)
	{
		case ENTITY_EVENT_UPDATE:
			Update();
			break;
	}
}


void CItemInteractionComponent::SerializeProperties(Serialization::IArchive& archive)
{
	if (archive.isInput())
	{
		OnResetState();
	}
}


void CItemInteractionComponent::OnInteractionItemInspect()
{
	// We're already inspecting it, drop it instead.
	if (m_inspectionState == InspectionState::eInspecting)
	{
		OnInteractionItemDrop();
		return;
	}

	gEnv->pLog->LogAlways("OnInteractionItemInspect fired.");
	m_inspectionState = InspectionState::eInspecting;

	if (auto pCharacter = CPlayer::GetLocalCharacter())
	{
		auto pEntity = GetEntity();
		m_initialPosition = pEntity->GetPos();
		m_initialRotation = pEntity->GetRotation();
		m_targetPosition = pCharacter->GetEntity()->GetPos() + pCharacter->GetLocalLeftHandPos();
		m_timeInAir = 0.0f;
		m_timeInAirRequired = (m_targetPosition - m_initialPosition).GetLength() / kJumpToPlayerSpeed;

		// #TODO: Use a helper method instead of setting directly.
		if (auto pPlayer = CPlayer::GetLocalPlayer())
		{
			pPlayer->SetCharacterInteractionMode(true);
		}
	}
}


void CItemInteractionComponent::OnInteractionItemPickup()
{
	// We're already inspecting it, drop it instead.
	if (m_inspectionState == InspectionState::ePickingUp)
	{
		OnInteractionItemDrop();
		return;
	}

	gEnv->pLog->LogAlways("OnInteractionItemPickup fired.");
	m_inspectionState = InspectionState::ePickingUp;

	if (auto pCharacter = CPlayer::GetLocalCharacter())
	{
		m_initialPosition = GetEntity()->GetPos();
		m_targetPosition = pCharacter->GetEntity()->GetPos() + pCharacter->GetLocalRightHandPos();
		m_timeInAir = 0.0f;
		m_timeInAirRequired = (m_targetPosition - m_initialPosition).GetLength() / kJumpToPlayerSpeed;

		// #TODO: Use a helper method instead of setting directly.
		if (auto pPlayer = CPlayer::GetLocalPlayer())
		{
			pPlayer->SetCharacterInteractionMode(true);
		}
	}
}


void CItemInteractionComponent::OnInteractionItemDrop()
{
	gEnv->pLog->LogAlways("OnInteractionItemDrop fired.");
	m_inspectionState = InspectionState::eDroping;

	if (auto pCharacter = CPlayer::GetLocalCharacter())
	{
		auto pEntity = GetEntity();

		pe_action_awake action;
		action.bAwake = true;

		// No impulse, we're just letting it drop.
		IPhysicalEntity* pPhysEntity = pEntity->GetPhysics();
		if (pPhysEntity)
			pPhysEntity->Action(&action);
	}

	if (auto pPlayer = CPlayer::GetLocalPlayer())
	{
		pPlayer->SetCharacterInteractionMode(false);
	}
}


void CItemInteractionComponent::OnInteractionItemToss()
{
	gEnv->pLog->LogAlways("OnInteractionItemToss fired.");
	m_inspectionState = InspectionState::eTossing;

	if (auto pCharacter = CPlayer::GetLocalCharacter())
	{
		auto pEntity = GetEntity();
		Vec3 impulse = pCharacter->GetEntity()->GetRotation() * FORWARD_DIRECTION * kTossFactor;

		// A small impulse to toss it aside.
		pe_action_impulse action;
		action.impulse = impulse;
		action.point = pCharacter->GetLocalRightHandPos();

		IPhysicalEntity* pPhysEntity = pEntity->GetPhysics();
		if (pPhysEntity)
			pPhysEntity->Action(&action);
	}

	if (auto pPlayer = CPlayer::GetLocalPlayer())
	{
		pPlayer->SetCharacterInteractionMode(false);
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

	if (auto pCharacter = CPlayer::GetLocalCharacter())
	{
		Vec3 delta = (m_targetPosition - m_initialPosition) * (1.0f - (m_timeInAir / m_timeInAirRequired));
		pEntity->SetPos(m_targetPosition - delta);

		// Allow them to rotate the item in their hands.
		if (auto pPlayerInput = CPlayer::GetLocalPlayer()->GetPlayerInput())
		{
			// #TODO: The rotation flips the controls when the item is upside down. That feels weird. Is there a way to remove this
			// from the rotation?
			const auto inputRotation = Quat(Ang3(pPlayerInput->GetPitchDelta() * kInspectionRotationFactor, pPlayerInput->GetYawDelta() * kInspectionRotationFactor, 0.0f));
			const auto characterRotation = Quat(Ang3(0.0f, 0.0f, pCharacter->GetEntity()->GetRotation().GetFwdZ())).GetNormalized();
			pEntity->SetRotation(pEntity->GetRotation() * inputRotation * characterRotation);
		}
	}
}


void CItemInteractionComponent::OnPickingUpUpdate(const float frameTime)
{
	const auto pEntity = GetEntity();

	// For now, just pull the object to a target location.
	m_timeInAir = min(m_timeInAirRequired, m_timeInAir + frameTime);

	if (auto pCharacter = CPlayer::GetLocalCharacter())
	{
		Vec3 delta = (m_targetPosition - m_initialPosition) * (1.0f - (m_timeInAir / m_timeInAirRequired));
		pEntity->SetPos(m_targetPosition - delta);
	}

	// Allow them to rotate the item in their hands.
	if (auto pPlayerInput = CPlayer::GetLocalPlayer()->GetPlayerInput())
	{
		const auto rotation = Quat(Ang3(pPlayerInput->GetHeadPitchDelta(), 0.0f, pPlayerInput->GetYawDelta()));
		pEntity->SetRotation(pEntity->GetRotation() * rotation);
	}
}
