#include <StdAfx.h>

#include "Player.h"
#include <CryMath/Cry_Math.h>
#include <IAnimatedCharacter.h>
#include <IGameObject.h>
#include <IItemSystem.h>
#include <Game/Game.h>
#include <Game/GameRules.h>
#include <CryInput/IHardwareMouse.h>
#include <Item/Item.h>
#include <Item/Weapon/Weapon.h>
#include <Actor/Character/Character.h>
#include <Player/Camera/ICameraComponent.h>
#include <Player/Camera/ICameraManagerComponent.h>
#include <Player/Input/PlayerInputComponent.h>
#include <Game/GameFactory.h>


class CPlayerRegistrator
	: public IEntityRegistrator
	, public CPlayer::SExternalCVars
{
	virtual void Register() override
	{
		CGameFactory::RegisterGameObject<CPlayer>("Player");

		RegisterCVars();
	}

	void RegisterCVars()
	{
		REGISTER_CVAR2("pl_mass", &m_mass, 90.f, VF_CHEAT, "Mass of the player entity in kg");
	}
};

CPlayerRegistrator g_playerRegistrator;


CPlayer::~CPlayer()
{
	// Very important that this gets called. Removes the player from the system.
	gEnv->pGame->GetIGameFramework()->GetIActorSystem()->RemoveActor(GetEntityId());
}


// ***
// *** IGameObjectExtension
// ***


bool CPlayer::Init(IGameObject * pGameObject)
{
	SetGameObject(pGameObject);

	return pGameObject->BindToNetwork();
}


void CPlayer::PostInit(IGameObject * pGameObject)
{
	// Registers this instance to the actor system.
	// TODO: Can we have a player who isn't an actor?
	gEnv->pGame->GetIGameFramework()->GetIActorSystem()->AddActor(GetEntityId(), this);

	// Create a camera manager for this player. We do this early, since character attachment code needs to make calls
	// to a functioning camera.
	m_pCameraManager = static_cast<ICameraManagerComponent*> (GetGameObject()->AcquireExtension("CameraManager"));

	// Acquire a player input component. At a later time it will be useful to check if a network version is needed, or
	// perhaps AI / NULL versions.
	m_pPlayerInput = static_cast<IPlayerInputComponent*> (GetGameObject()->AcquireExtension("PlayerInput"));

	// HACK: hard coded effort to grab an entity to attach to as our pawn / character. It must occur after the camera has a chance to create itself.
	auto pCharacterEntity = gEnv->pEntitySystem->FindEntityByName("Character1");
	CRY_ASSERT_MESSAGE(pCharacterEntity, "Player is not attached to a character. Do not enter game mode without attaching to a character.");
	if (pCharacterEntity)
		AttachToCharacter(pCharacterEntity->GetId());

	// Register for game object events.
	RegisterForGOEvents();
}


bool CPlayer::ReloadExtension(IGameObject * pGameObject, const SEntitySpawnParams &params)
{
	//// Unregister from game object events.
	//pGameObject->UnRegisterExtForEvents(this, nullptr, 0);

	//// Restores this instance's game object in case it has changed.
	//ResetGameObject();

	//// Add this instance to the network.
	//if (!GetGameObject()->BindToNetwork())
	//	return false;

	//// Removes this instance's previous actor and adds it as a new one.
	//// TODO: Can we have a player who isn't an actor?
	//gEnv->pGame->GetIGameFramework()->GetIActorSystem()->RemoveActor(params.prevId);
	//gEnv->pGame->GetIGameFramework()->GetIActorSystem()->AddActor(GetEntityId(), this);

	//// Register for game object events again.
	//RegisterForGOEvents();

	return true;
}


void CPlayer::HandleEvent(const SGameObjectEvent& event)
{
	switch (event.event)
	{
		// Called automatically by various systems when this instance should become the client actor.
		// TODO: I don't think the character is receiving this event presently, so we might need to push it down
		// to the attached character at some point.
		case eGFE_BecomeLocalPlayer:
		{
			//// We want to warn if they enter game mode without being attached to a character.
			//CRY_ASSERT_MESSAGE(GetAttachedActor(), "Player is not attached to a character. Do not enter game mode without attaching to a character.");

			// Store that we are now the client actor.
			m_isClient = true;

			gEnv->pLog->LogAlways("CPlayer::HandleEvent(): Entity \"%s\" became the local player!", GetEntity()->GetName());
		}
		break;

		default:
			break;
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

void CPlayer::RegisterForGOEvents()
{
	// Lists the game object events we want to be notified about.
	const int EventsToRegister [] =
	{
		eGFE_BecomeLocalPlayer,		// Become client actor events.
	};

	// Register for the specified game object events.
	GetGameObject()->RegisterExtForEvents(this, EventsToRegister, sizeof(EventsToRegister) / sizeof(int));
}


void CPlayer::AttachToCharacter(EntityId characterId)
{
	// Track these for our use.
	m_attachedCharacterId = characterId;
	m_cameraTargetId = characterId;

	// Inform the camera system we are now watching a different entity.
	m_pCameraManager->AttachToEntity(characterId);

	// Ensure the object is a character, then inform it we are now attached.
	// TODO: put in a check to ensure only characters will ever be cast.
	auto pCharacter = static_cast<CCharacter*>(gEnv->pGame->GetIGameFramework()->GetIActorSystem()->GetActor(characterId));
	if (pCharacter)
		pCharacter->OnPlayerAttach(*this);
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

	return gEnv->pGame->GetIGameFramework()->GetIActorSystem()->GetActor(m_attachedCharacterId);
}


CCharacter* CPlayer::GetAttachedCharacter() const
{
	CRY_ASSERT_MESSAGE(m_attachedCharacterId != INVALID_ENTITYID, "There is no character attached to this player.");

	return static_cast<CCharacter*> (gEnv->pGame->GetIGameFramework()->GetIActorSystem()->GetActor(m_attachedCharacterId));
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


ICameraManagerComponent* CPlayer::GetCameraManager() const
{
//	CRY_ASSERT_MESSAGE(m_pCameraManager, "A camera manager is critical when the level contains actors.");
	return m_pCameraManager;
}


ICameraComponent* CPlayer::GetCamera() const
{
//	CRY_ASSERT_MESSAGE(m_pCameraManager, "A camera manager is critical when the level contains actors.");
	return m_pCameraManager ? m_pCameraManager->GetCamera() : nullptr;
}
