#include <StdAfx.h>

#include "Player.h"
#include <Components/Items/ItemComponent.h>
#include <Item/Weapon/Weapon.h>
#include <Actor/Character/Character.h>
#include "Camera/CameraManagerComponent.h"
#include <Components/Player/Input/PlayerInputComponent.h>


namespace Chrysalis
{
void CPlayerComponent::Register(Schematyc::CEnvRegistrationScope& componentScope)
{
}


void CPlayerComponent::ReflectType(Schematyc::CTypeDesc<CPlayerComponent>& desc)
{
	desc.SetGUID(CPlayerComponent::IID());
	desc.SetEditorCategory("Actors");
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
	//auto gameObject = GetGameObject();
	m_pPlayerInput = pEntity->CreateComponent<CPlayerInputComponent>();

	// #HACK: #TODO: Is this right? We only want to register action maps on the local client.
	if (GetEntityId() == gEnv->pGameFramework->GetClientActorId())
	{
		m_isClient = true;
		gEnv->pLog->LogAlways("CPlayerComponent::HandleEvent(): Entity \"%s\" became the local player!", GetEntity()->GetName());
	}
}


// ***
// *** CPlayerComponent
// ***


bool CPlayerComponent::IsThirdPerson() const
{
	return m_pCameraManager->IsThirdPerson();
}


void CPlayerComponent::OnToggleThirdPerson()
{
	//// Track first / third person for each actor.
	m_pCameraManager->ToggleThirdPerson();

	// If we switched view modes, we may need to load a new animation context.
	if (auto pCharacter = GetLocalCharacter())
	{
		pCharacter->OnResetState();
	}
}


void CPlayerComponent::AttachToCharacter(EntityId characterId)
{
	// Track these for our use.
	m_attachedCharacterId = characterId;
	m_cameraTargetId = characterId;

	// Inform the camera system we are now watching a different entity.
	m_pCameraManager->AttachToEntity(characterId);

	// Ensure the object is a character, then inform it we are now attached.
	// #TODO: put in a check to ensure only characters will ever be cast.
	auto pCharacter = static_cast<CCharacterComponent*>(CActor::GetActor(characterId));
	if (pCharacter)
	{
		pCharacter->OnPlayerAttach(*this);

		// Flip the camera between FP / TP if needed.
		if (pCharacter->IsThirdPerson() != IsThirdPerson())
			OnToggleThirdPerson();
	}
}


void CPlayerComponent::AttachToCharacter()
{
	// #HACK: hard coded effort to grab an entity to attach to as our pawn / character. It must occur after the camera has a chance to create itself.
	auto pCharacterEntity = gEnv->pEntitySystem->FindEntityByName("Hero");
	CRY_ASSERT_MESSAGE(pCharacterEntity, "Player is not attached to a character. Do not enter game mode without attaching to a character.");
	if (pCharacterEntity)
		AttachToCharacter(pCharacterEntity->GetId());
}


EntityId CPlayerComponent::GetAttachedEntityId() const
{
	CRY_ASSERT_MESSAGE(m_attachedCharacterId != INVALID_ENTITYID, "There is no character attached to this player.");

	return m_attachedCharacterId;
}


IEntity* CPlayerComponent::GetAttachedEntity() const
{
	CRY_ASSERT_MESSAGE(m_attachedCharacterId != INVALID_ENTITYID, "There is no character attached to this player.");

	return gEnv->pEntitySystem->GetEntity(m_attachedCharacterId);
}


CCharacterComponent* CPlayerComponent::GetAttachedCharacter() const
{
	CRY_ASSERT_MESSAGE(m_attachedCharacterId != INVALID_ENTITYID, "There is no character attached to this player.");

	return static_cast<CCharacterComponent*> (CActor::GetActor(m_attachedCharacterId));
}


CCharacterComponent* CPlayerComponent::GetLocalCharacter()
{
	// Try and find a local player on this client.
	auto pPlayer = CPlayerComponent::GetLocalPlayer();
	if (pPlayer)
		return pPlayer->GetAttachedCharacter();

	CRY_ASSERT_MESSAGE(false, "Attempt to retrieve local character failed. Check your logic.");

	return nullptr;
}


CItemComponent* CPlayerComponent::GetLocalItem()
{
	// HACK: FIX: 5.4 for now, this is a broken concept. Fix it when adding the item system back in.

	// Try and find a local player on this client.
	//auto pPlayer = CPlayerComponent::GetLocalPlayer();
	//if (pPlayer)
	//	return static_cast<CItemComponent*>(pPlayer->GetCurrentItem());

	return nullptr;
}


CWeapon* CPlayerComponent::GetLocalWeapon()
{
	// HACK: FIX: 5.4 for now, this is a broken concept. Fix it when adding the item system back in.

	//// Try and find a local player on this client.
	//auto pItem = CPlayerComponent::GetLocalItem();
	//if (pItem)
	//	return static_cast<CWeapon*>(pItem->GetIWeapon());

	return nullptr;
}


void CPlayerComponent::Revive()
{
	if (auto pCharacter = GetLocalCharacter())
	{
		pCharacter->Revive();
	}
}


EntityId CPlayerComponent::GetCameraTargetId() const
{
	return m_cameraTargetId;
}


CCameraManagerComponent* CPlayerComponent::GetCameraManager() const
{
	//	CRY_ASSERT_MESSAGE(m_pCameraManager, "A camera manager is critical when the level contains actors.");
	return m_pCameraManager;
}


ICameraComponent* CPlayerComponent::GetCamera() const
{
	//	CRY_ASSERT_MESSAGE(m_pCameraManager, "A camera manager is critical when the level contains actors.");
	return m_pCameraManager ? m_pCameraManager->GetCamera() : nullptr;
}
}