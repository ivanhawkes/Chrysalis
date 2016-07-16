#include <StdAfx.h>

#include "Player.h"
#include <CryMath/Cry_Math.h>
#include <IAnimatedCharacter.h>
#include <IGameObject.h>
#include <IItemSystem.h>
#include <Game/Game.h>
#include <Game/Rules/GameRules.h>
#include <CryInput/IHardwareMouse.h>
#include <Actor/ActorClassTypes.h>
#include <Item/Item.h>
#include <Item/Weapon/Weapon.h>
#include <Camera/ICamera.h>
#include <Camera/ICameraManager.h>
#include <Actor/Character/Character.h>
#include <PlayerInput/PlayerInput.h>
#include <PlayerInput/IPlayerInput.h>
//#include <steam/steam_api.h>


CPlayer::CPlayer()
{}


CPlayer::~CPlayer()
{
	if (m_pCameraManager)
		GetGameObject()->ReleaseExtension("CameraManager");
	if (m_pPlayerInput)
		GetGameObject()->ReleaseExtension("PlayerInput");

	// Very important that this gets called. Removes the player from the system.
	gEnv->pGame->GetIGameFramework()->GetIActorSystem()->RemoveActor(GetEntityId());
}


// ***
// *** IGameObjectExtension
// ***


void CPlayer::GetMemoryUsage(ICrySizer *pSizer) const
{
	pSizer->Add(*this);
	pSizer->AddObject(m_pCameraManager);
	pSizer->AddObject(m_pPlayerInput);
}


bool CPlayer::Init(IGameObject * pGameObject)
{
	// Critical this is called.
	SetGameObject(pGameObject);

	/**
	Stores whether this instance is the client actor. A player might be connected through a network or at the PC;
	this will let us know when they are the local client.
	
	TODO: In HandleEvent this is just set straight to true - we need to unify the approach for dealing with local client.
	*/
	m_bClient = (((CGame*) gEnv->pGame)->GetClientActorID() == GetEntityId());

	IEntity *pEntity = GetEntity();
	IEntityClass *pEntityClass = pEntity->GetClass();

	// Registers this instance to the actor system.
	gEnv->pGame->GetIGameFramework()->GetIActorSystem()->AddActor(GetEntityId(), this);

	// Add this instance to the network or fail out early.
	if (!pGameObject->BindToNetwork())
		return false;

	// We will require pre-physics updates.
	pGameObject->EnablePrePhysicsUpdate(ePPU_Always);

	// Create a camera manager for this player. We do this early, since character attachment code needs to make calls
	// to a functioning camera.
	m_pCameraManager = reinterpret_cast<ICameraManager*> (GetGameObject()->AcquireExtension("CameraManager"));

	// Acquire a player input component. At a later time it will be useful to check if a network version is needed, or
	// perhaps AI / NULL versions.
	m_pPlayerInput = reinterpret_cast<IPlayerInput*> (GetGameObject()->AcquireExtension("PlayerInput"));

	// ***
	// *** Spawn ourselves a character we can attach onto.
	// ***

	//	// Setup the camera spawn parameters.
	//	SEntitySpawnParams spCharacter;
	//	
	//	// Entity will not be created through the entity pool.
	//	spCharacter.bCreatedThroughPool = false;
	//	
	//	// Spawn the character even though the entity system has blocked all spawning ability.
	//	spCharacter.bIgnoreLock = true;
	//
	//	// We need to tell the entity what class to use (which IGameObjectExtension to attach to it) 
	//	// but to make sure we search through all of the available classes, we need to move the class iterator to the beginning of the list.
	//	gEnv->pEntitySystem->GetClassRegistry ()->IteratorMoveFirst ();
	//
	//	// Tells this entity to attach the camera IGameObjectExtension to it.
	//	spCharacter.pClass = gEnv->pEntitySystem->GetClassRegistry ()->FindClass ("Character");
	//
	//	// Assign an unique name to this entity.
	//	// TODO: actually need this to be unique!
	//	spCharacter.sName = "PlayerCharacter";
	//
	//	// Tells this entity to have 0 rotation on all axis.
	//	spCharacter.qRotation = Quat (Ang3 (0, 0, 0));
	//
	//	// Spawn relative to the player.
	////	spCharacter.vPosition = GetEntity ()->GetWorldPos () + Vec3 (0.0f, 2.0f, 0.0f);
	//	spCharacter.vPosition = Vec3 (314.0f, 250.0f, 130.0f);
	//
	//	// Tells this entity to have default scale.
	//	spCharacter.vScale = Vec3 (1, 1, 1);
	//
	//	// Spawn an entity using the specified parameters.
	//	IEntity* pCharacterEntity = gEnv->pEntitySystem->SpawnEntity (spCharacter);
	//	if (!pCharacterEntity)
	//		return false;
	//
	//	// Gets the IGameObject associated with the newly spawned entity.
	//	auto* pCharacterGameObject = gEnv->pGame->GetIGameFramework ()->GetGameObject (pCharacterEntity->GetId ());
	//	if (!pCharacterGameObject)
	//		return false;
	//
	//	// TODO: set m_characterId here!!!
	//
	// 
	// 
	// 
	// 
	// 
	// 
	// // TODO: This is broken or likely broken. QueryExtension is meant to be called from PostInit().
	//	// Gets the camera IGameObjectExtension that's attached to the newly spawned entity's IGameObject.
	//	auto pCharacter = static_cast<CCharacter*>(pCharacterGameObject->QueryExtension ("Character"));
	//	if (!pCharacter)
	//		return false;

	// Set the target for the camera to this entity.
	//	pCharacter->SetTargetEntityId (GetEntityId ());

	// HACK: for now, camera target and character target can be this entity.
	//m_attachedCharacterId = GetEntityId ();
	//m_cameraTargetId = GetEntityId ();

	// We'll start with the player in third person mode. In future we might check to see which mode they want, but for
	// now, this is good enough.
	if (!IsThirdPerson())
		ToggleThirdPerson();

	// HACK: hard coded effort to grab an entity to attach to as our pawn / character. It must occur after the camera has a chance to create itself.
	auto pCharacterEntity = gEnv->pEntitySystem->FindEntityByName("Character1");
	if (pCharacterEntity)
	{
		AttachToCharacter(pCharacterEntity->GetId());
	}
	CRY_ASSERT_MESSAGE(pCharacterEntity, "Player is not attached to a character. Do not enter game mode without attaching to a character.");

	return true;
}


void CPlayer::PostInit(IGameObject * pGameObject)
{
	// Allow this instance to be updated every frame.
	pGameObject->EnableUpdateSlot(this, 0);

	// Allow this instance to be post-updated every frame.
	pGameObject->EnablePostUpdates(this);

	//if (m_teamId)
	//{
	//	CGameRules *pGameRules = g_pGame->GetGameRules ();
	//	pGameRules->ClDoSetTeam (m_teamId, GetEntityId ());
	//}

	// Register for game object events.
	RegisterForGOEvents();

	// Register for pre-physics update.
	RegisterEvent(ENTITY_EVENT_PREPHYSICSUPDATE, IComponent::EComponentFlags_Enable);

	// If we are the client actor than notify the game of us being spawned.
	if (m_bClient)
		((CGame*) gEnv->pGame)->OnClientActorSpawned(this);
}


void CPlayer::InitClient(int channelId)
{}


void CPlayer::PostInitClient(int channelId)
{}


bool CPlayer::ReloadExtension(IGameObject * pGameObject, const SEntitySpawnParams &params)
{
	// Unregister from game object events.
	pGameObject->UnRegisterExtForEvents(this, nullptr, 0);

	// Restores this instance's game object in case it has changed.
	ResetGameObject();

	// Add this instance to the network.
	if (!GetGameObject()->BindToNetwork())
		return false;

	// Removes this instance's previous actor and adds it as a new one.
	gEnv->pGame->GetIGameFramework()->GetIActorSystem()->RemoveActor(params.prevId);
	gEnv->pGame->GetIGameFramework()->GetIActorSystem()->AddActor(GetEntityId(), this);

	// Register for game object events again.
	RegisterForGOEvents();

	return true;
}


void CPlayer::PostReloadExtension(IGameObject * pGameObject, const SEntitySpawnParams &params)
{}


bool CPlayer::GetEntityPoolSignature(TSerialize signature)
{
	return true;
}


void CPlayer::Release()
{
	// Destroy this instance.
	delete this;
}


void CPlayer::FullSerialize(TSerialize ser)
{}


bool CPlayer::NetSerialize(TSerialize ser, EEntityAspects aspect, uint8 profile, int pflags)
{
	return true;
}


ISerializableInfoPtr CPlayer::GetSpawnInfo()
{
	return nullptr;
}


void CPlayer::Update(SEntityUpdateContext& ctx, int updateSlot)
{
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
			// We want to warn if they enter game mode without being attached to a character.
			CRY_ASSERT_MESSAGE(GetAttachedActor(), "Player is not attached to a character. Do not enter game mode without attaching to a character.");

			// Store that we are now the client actor.
			m_bClient = true;

			gEnv->pLog->LogAlways("CPlayer::HandleEvent(): Entity \"%s\" became the local player!", GetEntity()->GetName());
		}
			break;

		default:
			break;
	}
}


void CPlayer::ProcessEvent(SEntityEvent& event)
{
	switch (event.event)
	{
		// Called automatically at the start of every level.
		case ENTITY_EVENT_START_LEVEL:
			break;

		case ENTITY_EVENT_PREPHYSICSUPDATE:
			PrePhysicsUpdate();
			break;

		default:
			break;
	}
}


void CPlayer::PrePhysicsUpdate()
{
	const float frameTime = gEnv->pTimer->GetFrameTime();
}


void CPlayer::SetChannelId(uint16 id)
{}


void CPlayer::SetAuthority(bool auth)
{}


void CPlayer::PostUpdate(float frameTime)
{
}


void CPlayer::PostRemoteSpawn()
{}


// *** 
// *** IActor
// *** 

// *** In almost all cases we will want to pass along the responsibility for this to the actor which this player is
// *** presently attached to. Players should be attached to a character at nearly all times. This won't be true in
// *** character creation screens or other times, so that code will need to be careful how it acts.


void CPlayer::SetHealth(float health)
{
	if (auto pActor = GetAttachedActor())
		pActor->SetHealth(health);
}


float CPlayer::GetHealth() const
{
	auto pActor = GetAttachedActor();
	return pActor ? pActor->GetHealth() : 100.0f;
}


int	CPlayer::GetHealthAsRoundedPercentage() const
{
	auto pActor = GetAttachedActor();
	return pActor ? pActor->GetHealthAsRoundedPercentage() : 100;
}


void CPlayer::SetMaxHealth(float maxHealth)
{
	if (auto pActor = GetAttachedActor())
		pActor->SetMaxHealth(maxHealth);
}


float CPlayer::GetMaxHealth() const
{
	auto pActor = GetAttachedActor();
	return pActor ? pActor->GetMaxHealth() : 100.0f;
}


int CPlayer::GetArmor() const
{
	auto pActor = GetAttachedActor();
	return pActor ? pActor->GetArmor() : 100;
}


int	CPlayer::GetMaxArmor() const
{
	auto pActor = GetAttachedActor();
	return pActor ? pActor->GetMaxArmor() : 100;
}


bool CPlayer::IsFallen() const
{
	auto pActor = GetAttachedActor();
	return pActor ? pActor->IsFallen() : false;
}


bool CPlayer::IsDead() const
{
	auto pActor = GetAttachedActor();
	return pActor ? pActor->IsDead() : false;
}


int	CPlayer::IsGod()
{
	auto pActor = GetAttachedActor();
	return pActor ? pActor->IsGod() : false;
}


void CPlayer::Fall(Vec3 hitPos)
{
	auto pActor = GetAttachedActor();
	if (pActor)
		pActor->Fall(hitPos);
}


bool CPlayer::AllowLandingBob()
{
	auto pActor = GetAttachedActor();
	return pActor ? pActor->AllowLandingBob() : true;
}


void CPlayer::PlayAction(const char *action, const char *extension, bool looping)
{
	if (auto pActor = GetAttachedActor())
		pActor->PlayAction(action, extension, looping);
}


IAnimationGraphState* CPlayer::GetAnimationGraphState()
{
	auto pActor = GetAttachedActor();
	return pActor ? pActor->GetAnimationGraphState() : nullptr;
}


void CPlayer::ResetAnimationState()
{
	if (auto pActor = GetAttachedActor())
		pActor->ResetAnimationState();
}


void CPlayer::CreateScriptEvent(const char *event, float value, const char *str)
{
	if (auto pActor = GetAttachedActor())
		pActor->CreateScriptEvent(event, value, str);
}


bool CPlayer::BecomeAggressiveToAgent(EntityId entityID)
{
	auto pActor = GetAttachedActor();
	return pActor ? pActor->BecomeAggressiveToAgent(entityID) : false;
}


void CPlayer::SetFacialAlertnessLevel(int alertness)
{
	if (auto pActor = GetAttachedActor())
		pActor->SetFacialAlertnessLevel(alertness);
}


void CPlayer::RequestFacialExpression(const char* pExpressionName, f32* sequenceLength)
{
	if (auto pActor = GetAttachedActor())
		pActor->RequestFacialExpression(pExpressionName, sequenceLength);
}


void CPlayer::PrecacheFacialExpression(const char* pExpressionName)
{
	if (auto pActor = GetAttachedActor())
		pActor->PrecacheFacialExpression(pExpressionName);
}


EntityId CPlayer::GetGrabbedEntityId() const
{
	auto pActor = GetAttachedActor();
	return pActor ? pActor->GetGrabbedEntityId() : INVALID_ENTITYID;
}


void CPlayer::HideAllAttachments(bool isHiding)
{
	if (auto pActor = GetAttachedActor())
		pActor->HideAllAttachments(isHiding);
}


void CPlayer::SetIKPos(const char *pLimbName, const Vec3& goalPos, int priority)
{
	if (auto pActor = GetAttachedActor())
		pActor->SetIKPos(pLimbName, goalPos, priority);
}


void CPlayer::SetViewInVehicle(Quat viewRotation)
{
	if (auto pActor = GetAttachedActor())
		pActor->SetViewInVehicle(viewRotation);
}


void CPlayer::SetViewRotation(const Quat &rotation)
{
	if (auto pActor = GetAttachedActor())
		pActor->SetViewRotation(rotation);
}


Quat CPlayer::GetViewRotation() const
{
	auto pActor = GetAttachedActor();
	return pActor ? pActor->GetViewRotation() : IDENTITY;
}


bool CPlayer::IsFriendlyEntity(EntityId entityId, bool bUsingAIIgnorePlayer) const
{
	auto pActor = GetAttachedActor();
	return pActor ? pActor->IsFriendlyEntity(entityId, bUsingAIIgnorePlayer) : false;
}


Vec3 CPlayer::GetLocalEyePos() const
{
	auto pActor = GetAttachedActor();
	return pActor ? pActor->GetLocalEyePos() : Vec3(0.0f, 0.0f, 0.0f);
}


void CPlayer::CameraShake(float angle, float shift, float duration, float frequency, Vec3 pos, int ID, const char* source)
{
	// TODO: This needs to enable / disable as you flip from one character to another.
	if (auto pActor = GetAttachedActor())
		pActor->CameraShake(angle, shift, duration, frequency, pos, ID, source);
}


IItem* CPlayer::GetHolsteredItem() const
{
	auto pActor = GetAttachedActor();
	return pActor ? pActor->GetHolsteredItem() : nullptr;
}


void CPlayer::HolsterItem(bool holster, bool playSelect, float selectSpeedBias, bool hideLeftHandObject)
{
	if (auto pActor = GetAttachedActor())
		pActor->HolsterItem(holster, playSelect, selectSpeedBias, hideLeftHandObject);
}


IItem* CPlayer::GetCurrentItem(bool includeVehicle) const
{
	auto pActor = GetAttachedActor();
	return pActor ? pActor->GetCurrentItem(includeVehicle) : nullptr;
}


bool CPlayer::DropItem(EntityId itemId, float impulseScale, bool selectNext, bool byDeath)
{
	auto pActor = GetAttachedActor();
	return pActor ? pActor->DropItem(itemId, impulseScale, selectNext, byDeath) : false;
}


IInventory* CPlayer::GetInventory() const
{
	auto pActor = GetAttachedActor();
	return pActor ? pActor->GetInventory() : nullptr;
}


void CPlayer::NotifyCurrentItemChanged(IItem* newItem)
{
	if (auto pActor = GetAttachedActor())
		pActor->NotifyCurrentItemChanged(newItem);
}


IMovementController* CPlayer::GetMovementController() const
{
	auto pActor = GetAttachedActor();
	return pActor ? pActor->GetMovementController() : nullptr;
}


IEntity* CPlayer::LinkToVehicle(EntityId vehicleId)
{
	auto pActor = GetAttachedActor();
	return pActor ? pActor->LinkToVehicle(vehicleId) : nullptr;
}


IEntity* CPlayer::GetLinkedEntity() const
{
	auto pActor = GetAttachedActor();
	return pActor ? pActor->GetLinkedEntity() : nullptr;
}


uint8 CPlayer::GetSpectatorMode() const
{
	return 0;
}


bool CPlayer::IsThirdPerson() const
{
	// TODO: Decide if camera is player based or character based.
	return m_bIsThirdPerson;
}


void CPlayer::ToggleThirdPerson()
{
	// TODO: Decide if camera is player based or character based.
	m_bIsThirdPerson = !m_bIsThirdPerson;
}


bool CPlayer::IsPlayer() const
{
	return true;
}


bool CPlayer::IsClient() const
{
	auto pActor = GetAttachedActor();
	return pActor ? pActor->IsClient() : false;
}


bool CPlayer::IsMigrating() const
{
	auto pActor = GetAttachedActor();
	return pActor ? pActor->IsMigrating() : false;
}


void CPlayer::SetMigrating(bool isMigrating)
{
	if (auto pActor = GetAttachedActor())
		pActor->SetMigrating(isMigrating);
}


void CPlayer::InitLocalPlayer()
{
	if (auto pActor = GetAttachedActor())
		pActor->InitLocalPlayer();
}


const char* CPlayer::GetActorClassName() const
{
	// It's expecting the name of this class.
	return "CPlayer";
}


ActorClass CPlayer::GetActorClass() const
{
	return EACT_PLAYER;
}


const char* CPlayer::GetEntityClassName() const
{
	return GetEntity()->GetClass()->GetName();
}


void CPlayer::SerializeXML(XmlNodeRef& node, bool bLoading)
{
	// TODO: Think about how we handle this.
	CryLogAlways("[Error] CPlayer used as an IActor - %s - Line %d", __FILE__, __LINE__);
}


void CPlayer::SerializeLevelToLevel(TSerialize &ser)
{
	// TODO: Think about how we handle this.
	CryLogAlways("[Error] CPlayer used as an IActor - %s - Line %d", __FILE__, __LINE__);
}


IAnimatedCharacter* CPlayer::GetAnimatedCharacter()
{
	auto pActor = GetAttachedActor();
	return pActor ? pActor->GetAnimatedCharacter() : nullptr;
}


const IAnimatedCharacter* CPlayer::GetAnimatedCharacter() const
{
	auto pActor = GetAttachedActor();
	return pActor ? pActor->GetAnimatedCharacter() : nullptr;
}


void CPlayer::PlayExactPositioningAnimation(const char* sAnimationName, bool bSignal, const Vec3& vPosition, const Vec3& vDirection,
	float startWidth, float startArcAngle, float directionTolerance)
{
	if (auto pActor = GetAttachedActor())
		pActor->PlayExactPositioningAnimation(sAnimationName, bSignal, vPosition, vDirection, startWidth, startArcAngle, directionTolerance);
}


void CPlayer::CancelExactPositioningAnimation()
{
	if (auto pActor = GetAttachedActor())
		pActor->CancelExactPositioningAnimation();
}


void CPlayer::PlayAnimation(const char* sAnimationName, bool bSignal)
{
	if (auto pActor = GetAttachedActor())
		pActor->PlayAnimation(sAnimationName, bSignal);
}


void CPlayer::EnableTimeDemo(bool bTimeDemo)
{
	if (auto pActor = GetAttachedActor())
		pActor->EnableTimeDemo(bTimeDemo);
}


void CPlayer::SwitchDemoModeSpectator(bool activate)
{
	if (auto pActor = GetAttachedActor())
		pActor->SwitchDemoModeSpectator(activate);
}


IVehicle* CPlayer::GetLinkedVehicle() const
{
	auto pActor = GetAttachedActor();
	return pActor ? pActor->GetLinkedVehicle() : nullptr;
}


void CPlayer::OnAIProxyEnabled(bool enabled)
{
	if (auto pActor = GetAttachedActor())
		pActor->OnAIProxyEnabled(enabled);
}


void CPlayer::OnReturnedToPool()
{
	if (auto pActor = GetAttachedActor())
		pActor->OnReturnedToPool();
}


void CPlayer::OnPreparedFromPool()
{
	if (auto pActor = GetAttachedActor())
		pActor->OnPreparedFromPool();
}


bool CPlayer::ShouldMuteWeaponSoundStimulus() const
{
	auto pActor = GetAttachedActor();
	return pActor ? pActor->ShouldMuteWeaponSoundStimulus() : false;
}


void CPlayer::OnReused(IEntity *pEntity, SEntitySpawnParams &params)
{
	if (auto pActor = GetAttachedActor())
		pActor->OnReused(pEntity, params);
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

	return reinterpret_cast <CCharacter*> (gEnv->pGame->GetIGameFramework()->GetIActorSystem()->GetActor(m_attachedCharacterId));
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


ICameraManager* CPlayer::GetCameraManager() const
{
	return m_pCameraManager;
}


ICamera* CPlayer::GetCamera() const
{
	return m_pCameraManager->GetCamera();
}
