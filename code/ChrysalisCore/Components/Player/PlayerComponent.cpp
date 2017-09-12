#include <StdAfx.h>

#include "PlayerComponent.h"
#include <Components/Items/ItemComponent.h>
#include <Item/Weapon/Weapon.h>
#include <Actor/Character/Character.h>
#include "Camera/CameraManagerComponent.h"
#include <Components/Player/Input/PlayerInputComponent.h>
#include <Actor/ActorComponent.h>


namespace Chrysalis
{
void CPlayerComponent::Register(Schematyc::CEnvRegistrationScope& componentScope)
{
}


void CPlayerComponent::ReflectType(Schematyc::CTypeDesc<CPlayerComponent>& desc)
{
	desc.SetGUID(CPlayerComponent::IID());
	desc.SetEditorCategory("Player");
	desc.SetLabel("Player");
	desc.SetDescription("A local / remote player.");
	desc.SetIcon("icons:ObjectTypes/light.ico");
	desc.SetComponentFlags({ IEntityComponent::EFlags::Transform });
}


// ***
// *** IEntityComponent
// ***


void CPlayerComponent::Initialize()
{
	const auto pEntity = GetEntity();

	// Create a camera manager for this player. We do this early, since character attachment code needs to make calls
	// to a functioning camera.
	m_pCameraManager = pEntity->CreateComponent<CCameraManagerComponent>();

	// Acquire a player input component. At a later time it will be useful to check if a network version is needed, or
	// perhaps AI / NULL versions.
	// NOTE: This component requires a pointer to a camera manager - so it must always load after that component.
	m_pPlayerInput = pEntity->CreateComponent<CPlayerInputComponent>();
}


void CPlayerComponent::ProcessEvent(SEntityEvent& event)
{
	switch (event.event)
	{
		case ENTITY_EVENT_START_GAME:
		{
			// Revive the entity when gameplay starts
			Revive();
		}
		break;

		case ENTITY_EVENT_UPDATE:
		{
			SEntityUpdateContext* pCtx = (SEntityUpdateContext*)event.nParam [0];
		}
		break;
	}
}


bool CPlayerComponent::IsThirdPerson() const
{
	return m_pCameraManager->IsThirdPerson();
}


void CPlayerComponent::OnToggleThirdPerson()
{
	// Track first / third person for each actor.
	m_pCameraManager->ToggleThirdPerson();

	// If we switched view modes, we may need to load a new animation context.
	if (auto pActorComponent = GetLocalActor())
	{
		pActorComponent->OnToggleThirdPerson();
	}
}


void CPlayerComponent::AttachToCharacter(EntityId characterId)
{
	// Ensure the object is an actor, then inform it we are now attached.
	if (auto pAttachedEntity = gEnv->pEntitySystem->GetEntity(characterId))
	{
		if (auto pActorComponent = pAttachedEntity->GetComponent<CActorComponent>())
		{
			// If the target was an actor then it's safe to complete the attachment.
			m_attachedCharacterId = m_cameraTargetId = characterId;
			pActorComponent->OnPlayerAttach(*this);

			// Inform the camera system we are now watching a different entity.
			m_pCameraManager->AttachToEntity(m_cameraTargetId);

			// Flip the camera between FP / TP if needed.
			// TODO: Needed or not?
			//if (pActorComponent->IsThirdPerson() != IsThirdPerson())
			//	OnToggleThirdPerson();
		}
	}
}


void CPlayerComponent::AttachToHero()
{
	// #HACK: hard coded effort to grab an entity to attach to as our pawn / character. It must occur after the camera has a chance to create itself.
	auto pCharacterEntity = gEnv->pEntitySystem->FindEntityByName("Hero");
	CRY_ASSERT_MESSAGE(pCharacterEntity, "Player is not attached to a character. Do not enter game mode without attaching to a character.");
	if (pCharacterEntity)
		AttachToCharacter(pCharacterEntity->GetId());
}


EntityId CPlayerComponent::GetAttachedEntityId() const
{
	CRY_ASSERT_MESSAGE(m_attachedCharacterId != INVALID_ENTITYID, "This player is not attached to an entity.");

	return m_attachedCharacterId;
}


IEntity* CPlayerComponent::GetAttachedEntity() const
{
	CRY_ASSERT_MESSAGE(m_attachedCharacterId != INVALID_ENTITYID, "This player is not attached to an entity.");

	return gEnv->pEntitySystem->GetEntity(m_attachedCharacterId);
}


IActorComponent * CPlayerComponent::GetAttachedActor() const
{
	if (auto pAttachedEntity = GetAttachedEntity())
	{
		return pAttachedEntity->GetComponent<CActorComponent>();
	}

	CRY_ASSERT_MESSAGE(false, "Attempt to retrieve attached actor failed. Check your logic.");

	return nullptr;
}


IActorComponent* CPlayerComponent::GetLocalActor()
{
	// Try and find a local player on this client.
	auto pPlayer = CPlayerComponent::GetLocalPlayer();
	if (pPlayer)
		return pPlayer->GetAttachedActor();

	CRY_ASSERT_MESSAGE(false, "Attempt to retrieve local character failed. Check your logic.");

	return nullptr;
}


void CPlayerComponent::Revive()
{
	if (auto pActorComponent = GetLocalActor())
	{
		pActorComponent->Revive();
	}
}


EntityId CPlayerComponent::GetCameraTargetId() const
{
	return m_cameraTargetId;
}


CCameraManagerComponent* CPlayerComponent::GetCameraManager() const
{
	//CRY_ASSERT_MESSAGE(m_pCameraManager, "A camera manager is critical when the level contains actors.");
	return m_pCameraManager;
}


ICameraComponent* CPlayerComponent::GetCamera() const
{
	//	CRY_ASSERT_MESSAGE(m_pCameraManager, "A camera manager is critical when the level contains actors.");
	return m_pCameraManager ? m_pCameraManager->GetCamera() : nullptr;
}
}