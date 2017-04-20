#include <StdAfx.h>

#include "Player.h"
#include <Item/Item.h>
#include <Item/Weapon/Weapon.h>
#include <Actor/Character/Character.h>
#include <Player/Camera/CameraManagerComponent.h>
#include <Player/Input/PlayerInputComponent.h>


CRYREGISTER_CLASS(CPlayer)


class CPlayerRegistrator : public IEntityRegistrator
{
	virtual void Register() override
	{
		CChrysalisCorePlugin::RegisterEntityWithDefaultComponent<CPlayer>("Player");
		//RegisterEntityWithDefaultComponent<CPlayer>("Player", "Player", "character.bmp");

		// This should make the entity class invisible in the editor.
		auto cls = gEnv->pEntitySystem->GetClassRegistry()->FindClass("Player");
		cls->SetFlags(cls->GetFlags() | ECLF_INVISIBLE);
	}
};

CPlayerRegistrator g_playerRegistrator;


CPlayer::~CPlayer()
{
	// Very important that this gets called. Removes the player from the system.
	gEnv->pGameFramework->GetIActorSystem()->RemoveActor(GetEntityId());
}


// ***
// *** IEntityComponent
// ***


void CPlayer::Initialize()
{
}


void CPlayer::PostInit(IGameObject* pGameObject)
{
//	ISimpleActor::PostInit(pGameObject);

	auto pEntity = GetEntity();

	// Registers this instance to the actor system.
	// #TODO: Can we have a player who isn't an actor?
	gEnv->pGameFramework->GetIActorSystem()->AddActor(GetEntityId(), this);

	// Create a camera manager for this player. We do this early, since character attachment code needs to make calls
	// to a functioning camera.
	m_pCameraManager = static_cast<CCameraManagerComponent*> (GetGameObject()->AcquireExtension("CameraManager"));
	//m_pCameraManager = pEntity->CreateComponent<CCameraManagerComponent>();

	// Acquire a player input component. At a later time it will be useful to check if a network version is needed, or
	// perhaps AI / NULL versions.
	// NOTE: This component requires a pointer to a camera manager - so it must always load after that component.
	//auto gameObject = GetGameObject();
	m_pPlayerInput = static_cast<IPlayerInputComponent*> (GetGameObject()->AcquireExtension("PlayerInput"));
	//m_pPlayerInput = pEntity->CreateComponent<CPlayerInputComponent>();

	// #HACK: #TODO: Is this right? We only want to register action maps on the local client.
	if (GetEntityId() == gEnv->pGameFramework->GetClientActorId())
	{
		m_isClient = true;
		gEnv->pLog->LogAlways("CPlayer::HandleEvent(): Entity \"%s\" became the local player!", GetEntity()->GetName());
	}
}


// *** 
// *** IActor
// *** 


bool CPlayer::IsClient() const
{
	auto pActor = GetAttachedActor();
	return pActor ? pActor->IsClient() : false;
}


void CPlayer::InitLocalPlayer()
{
	if (auto pActor = GetAttachedActor())
		pActor->InitLocalPlayer();
}


bool CPlayer::IsThirdPerson() const
{
	return m_pCameraManager->IsThirdPerson();
}


void CPlayer::ToggleThirdPerson()
{
	m_pCameraManager->ToggleThirdPerson();
}


// ***
// *** CPlayer
// ***


void CPlayer::AttachToCharacter(EntityId characterId)
{
	// Track these for our use.
	m_attachedCharacterId = characterId;
	m_cameraTargetId = characterId;

	// Inform the camera system we are now watching a different entity.
	m_pCameraManager->AttachToEntity(characterId);

	// Ensure the object is a character, then inform it we are now attached.
	// #TODO: put in a check to ensure only characters will ever be cast.
	auto pCharacter = static_cast<CCharacter*>(gEnv->pGameFramework->GetIActorSystem()->GetActor(characterId));
	if (pCharacter)
		pCharacter->OnPlayerAttach(*this);
}


void CPlayer::AttachToCharacter()
{
	// #HACK: hard coded effort to grab an entity to attach to as our pawn / character. It must occur after the camera has a chance to create itself.
	auto pCharacterEntity = gEnv->pEntitySystem->FindEntityByName("Hero");
	CRY_ASSERT_MESSAGE(pCharacterEntity, "Player is not attached to a character. Do not enter game mode without attaching to a character.");
	if (pCharacterEntity)
		AttachToCharacter(pCharacterEntity->GetId());
}


EntityId CPlayer::GetAttachedEntityId() const
{
	CRY_ASSERT_MESSAGE(m_attachedCharacterId != INVALID_ENTITYID, "There is no character attached to this player.");

	return m_attachedCharacterId;
}


IEntity* CPlayer::GetAttachedEntity() const
{
	CRY_ASSERT_MESSAGE(m_attachedCharacterId != INVALID_ENTITYID, "There is no character attached to this player.");

	return gEnv->pEntitySystem->GetEntity(m_attachedCharacterId);
}


IActor* CPlayer::GetAttachedActor() const
{
	CRY_ASSERT_MESSAGE(m_attachedCharacterId != INVALID_ENTITYID, "There is no character attached to this player.");

	return gEnv->pGameFramework->GetIActorSystem()->GetActor(m_attachedCharacterId);
}


CCharacter* CPlayer::GetAttachedCharacter() const
{
	CRY_ASSERT_MESSAGE(m_attachedCharacterId != INVALID_ENTITYID, "There is no character attached to this player.");

	return static_cast<CCharacter*> (gEnv->pGameFramework->GetIActorSystem()->GetActor(m_attachedCharacterId));
}


CCharacter* CPlayer::GetLocalCharacter()
{
	// Try and find a local player on this client.
	auto pPlayer = CPlayer::GetLocalPlayer();
	if (pPlayer)
		return pPlayer->GetAttachedCharacter();

	CRY_ASSERT_MESSAGE(false, "Attempt to retrieve local character failed. Check your logic.");

	return nullptr;
}


CItem* CPlayer::GetLocalItem()
{
	// Try and find a local player on this client.
	auto pPlayer = CPlayer::GetLocalPlayer();
	if (pPlayer)
		return static_cast<CItem*>(pPlayer->GetCurrentItem());

	return nullptr;
}


CWeapon* CPlayer::GetLocalWeapon()
{
	// Try and find a local player on this client.
	auto pItem = CPlayer::GetLocalItem();
	if (pItem)
		return static_cast<CWeapon*>(pItem->GetIWeapon());

	return nullptr;
}


EntityId CPlayer::GetCameraTargetId() const
{
	return m_cameraTargetId;
}


CCameraManagerComponent* CPlayer::GetCameraManager() const
{
//	CRY_ASSERT_MESSAGE(m_pCameraManager, "A camera manager is critical when the level contains actors.");
	return m_pCameraManager;
}


ICameraComponent* CPlayer::GetCamera() const
{
//	CRY_ASSERT_MESSAGE(m_pCameraManager, "A camera manager is critical when the level contains actors.");
	return m_pCameraManager ? m_pCameraManager->GetCamera() : nullptr;
}

