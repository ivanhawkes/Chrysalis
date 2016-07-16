#include "StdAfx.h"

#include <Cry_Math.h>
#include "ActorTemplate.h"
#include <IViewSystem.h>
#include <IAnimatedCharacter.h>
#include <IGameObject.h>
#include <IItemSystem.h>
#include "Game/Game.h"
#include "Utility/MathUtils.h"
#include "Game/EventSystem/IWindowEventSystem.h"
#include <IHardwareMouse.h>
#include "Actor/PlayerCamera/FirstPersonCamera.h"
#include "Actor/PlayerCamera/ActionRPGCamera.h"
#include "Actor/ActorClassTypes.h"


// ***
// *** IGameObjectExtension
// ***


void CActorTemplate::GetMemoryUsage(ICrySizer *pSizer) const
{
	pSizer->Add(*this);
}


bool CActorTemplate::Init(IGameObject * pGameObject)
{
	// Stores the specified IGameObject in this instance.
	SetGameObject(pGameObject);

	// Stores whether this instance is the client actor.
	m_bClient = (((CGame*) gEnv->pGame)->GetClientActorID() == GetEntityId());

	IEntity *pEntity = GetEntity();
	IEntityClass *pEntityClass = pEntity->GetClass();

	// Stores whether this instance is using the "Player" class.
	m_bPlayer = (pEntityClass == gEnv->pEntitySystem->GetClassRegistry()->FindClass("Player"));

	// Registers this instance to the actor system.
	gEnv->pGame->GetIGameFramework()->GetIActorSystem()->AddActor(GetEntityId(), this);

	// Add this instance to the network.
	if (!pGameObject->BindToNetwork())
		return false;// Failed to add this instance to the network.

	return true;
}


void CActorTemplate::PostInit(IGameObject * pGameObject)
{
	// Allow this instance to be updated every frame.
	pGameObject->EnableUpdateSlot(this, 0);

	// Allow this instance to be post-updated every frame.
	pGameObject->EnablePostUpdates(this);

	// Register for game object events.
	RegisterForGOEvents();

	// If we are the client actor than notify the game of us being spawned.
	if (m_bClient)
		((CGame*) gEnv->pGame)->OnClientActorSpawned(this);
}


void CActorTemplate::InitClient(int channelId)
{}


void CActorTemplate::PostInitClient(int channelId)
{}


bool CActorTemplate::ReloadExtension(IGameObject * pGameObject, const SEntitySpawnParams &params)
{
	// Unregister from game object events.
	pGameObject->UnRegisterExtForEvents(this, nullptr, 0);

	// Restores this instance's game object in case it has changed.
	ResetGameObject();

	// Add this instance to the network.
	if (!GetGameObject()->BindToNetwork())
		return false;

	// Removes this instance's previous actor and adds it's new one.
	gEnv->pGame->GetIGameFramework()->GetIActorSystem()->RemoveActor(params.prevId);
	gEnv->pGame->GetIGameFramework()->GetIActorSystem()->AddActor(GetEntityId(), this);

	// Register for game object events again.
	RegisterForGOEvents();

	return true;
}


void CActorTemplate::PostReloadExtension(IGameObject * pGameObject, const SEntitySpawnParams &params)
{}


bool CActorTemplate::GetEntityPoolSignature(TSerialize signature)
{
	return true;
}


void CActorTemplate::Release()
{
	// Destroy this instance.
	delete this;
}


void CActorTemplate::FullSerialize(TSerialize ser)
{}


bool CActorTemplate::NetSerialize(TSerialize ser, EEntityAspects aspect, uint8 profile, int pflags)
{
	return true;
}


void CActorTemplate::PostSerialize()
{}


void CActorTemplate::SerializeSpawnInfo(TSerialize ser)
{}


ISerializableInfoPtr CActorTemplate::GetSpawnInfo()
{
	return nullptr;
}


void CActorTemplate::Update(SEntityUpdateContext& ctx, int updateSlot)
{}


void CActorTemplate::HandleEvent(const SGameObjectEvent& event)
{
	switch (event.event)
	{
		default:
			break;
	}
}


void CActorTemplate::ProcessEvent(SEntityEvent& event)
{
	switch (event.event)
	{
		default:
			break;
	}
}


void CActorTemplate::SetChannelId(uint16 id)
{}


void CActorTemplate::SetAuthority(bool auth)
{}


const void* CActorTemplate::GetRMIBase() const
{
	return CGameObjectExtensionHelper::GetRMIBase();
}


void CActorTemplate::PostUpdate(float frameTime)
{}


void CActorTemplate::PostRemoteSpawn()
{}


// ***
// *** CActorTemplate
// ***


CActorTemplate::CActorTemplate() :
m_bClient(false),
m_bPlayer(false),
m_bIsThirdPerson(false)
{}


CActorTemplate::~CActorTemplate()
{
	// Very important that this gets called. Removes the player from the system.
	gEnv->pGame->GetIGameFramework()->GetIActorSystem()->RemoveActor(GetEntityId());
}


void CActorTemplate::SetHealth(float health)
{}


float CActorTemplate::GetHealth() const
{
	return 100;
}


int	CActorTemplate::GetHealthAsRoundedPercentage() const
{
	return 100;
}


void CActorTemplate::SetMaxHealth(float maxHealth)
{}


float CActorTemplate::GetMaxHealth() const
{
	return 100;
}


int CActorTemplate::GetArmor() const
{
	return 100;
}


int	CActorTemplate::GetMaxArmor() const
{
	return 100;
}


bool CActorTemplate::IsFallen() const
{
	return false;
}


bool CActorTemplate::IsDead() const
{
	return false;
}


int	CActorTemplate::IsGod()
{
	return true;
}


void CActorTemplate::Fall(Vec3 hitPos)
{}


bool CActorTemplate::AllowLandingBob()
{
	return true;
}


void CActorTemplate::PlayAction(const char *action, const char *extension, bool looping)
{}


IAnimationGraphState* CActorTemplate::GetAnimationGraphState()
{
	return nullptr;
}


void CActorTemplate::ResetAnimationState()
{}


void CActorTemplate::CreateScriptEvent(const char *event, float value, const char *str)
{}


bool CActorTemplate::BecomeAggressiveToAgent(EntityId entityID)
{
	return true;
}


void CActorTemplate::SetFacialAlertnessLevel(int alertness)
{}


void CActorTemplate::RequestFacialExpression(const char* pExpressionName, f32* sequenceLength)
{}


void CActorTemplate::PrecacheFacialExpression(const char* pExpressionName)
{}


EntityId CActorTemplate::GetGrabbedEntityId() const
{
	return -1;
}


void CActorTemplate::HideAllAttachments(bool isHiding)
{}


void CActorTemplate::SetIKPos(const char *pLimbName, const Vec3& goalPos, int priority)
{}


void CActorTemplate::SetViewInVehicle(Quat viewRotation)
{}


void CActorTemplate::SetViewRotation(const Quat &rotation)
{}


Quat CActorTemplate::GetViewRotation() const
{
	return GetEntity()->GetWorldRotation();
}


bool CActorTemplate::IsFriendlyEntity(EntityId entityId, bool bUsingAIIgnoreActor) const
{
	return true;
}


Vec3 CActorTemplate::GetLocalEyePos() const
{
	// TODO: Where should the actor's eyes be positioned.
	return Vec3(0.0f, 0.0f, 1.82f);
}


void CActorTemplate::CameraShake(float angle, float shift, float duration, float frequency, Vec3 pos, int ID, const char* source)
{}


IItem* CActorTemplate::GetHolsteredItem() const
{
	return nullptr;
}


void CActorTemplate::HolsterItem(bool holster, bool playSelect, float selectSpeedBias, bool hideLeftHandObject)
{}


IItem* CActorTemplate::GetCurrentItem(bool includeVehicle) const
{
	return nullptr;
}


bool CActorTemplate::DropItem(EntityId itemId, float impulseScale, bool selectNext, bool byDeath)
{
	return true;
}


IInventory* CActorTemplate::GetInventory() const
{
	return nullptr;
}


void CActorTemplate::NotifyCurrentItemChanged(IItem* newItem)
{}


IMovementController* CActorTemplate::GetMovementController() const
{
	return nullptr;
}


IEntity* CActorTemplate::LinkToVehicle(EntityId vehicleId)
{
	return nullptr;
}


IEntity* CActorTemplate::GetLinkedEntity() const
{
	return nullptr;
}


uint8 CActorTemplate::GetSpectatorMode() const
{
	return 0;
}


bool CActorTemplate::IsThirdPerson() const
{
	return m_bIsThirdPerson;
}


void CActorTemplate::ToggleThirdPerson()
{
	m_bIsThirdPerson = !m_bIsThirdPerson;
}


bool CActorTemplate::IsPlayer() const
{
	return m_bPlayer;
}


bool CActorTemplate::IsClient() const
{
	return m_bClient;
}


bool CActorTemplate::IsMigrating() const
{
	return false;
}


void CActorTemplate::SetMigrating(bool isMigrating)
{}


void CActorTemplate::InitLocalPlayer()
{}


const char* CActorTemplate::GetActorClassName() const
{
	return "CActorTemplate";
}


ActorClass CActorTemplate::GetActorClass() const
{
	//#error "Give this a value."

	// TODO: You need to give this a unique value for each class or bad things will happen.
	return EACT_ACTOR;
}


const char* CActorTemplate::GetEntityClassName() const
{
	return GetEntity()->GetClass()->GetName();
}


void CActorTemplate::SerializeXML(XmlNodeRef& node, bool bLoading)
{}


void CActorTemplate::SerializeLevelToLevel(TSerialize &ser)
{}


IAnimatedCharacter* CActorTemplate::GetAnimatedCharacter()
{
	return nullptr;
}


const IAnimatedCharacter* CActorTemplate::GetAnimatedCharacter() const
{
	return nullptr;
}


void CActorTemplate::PlayExactPositioningAnimation(const char* sAnimationName, bool bSignal, const Vec3& vPosition, const Vec3& vDirection, float startWidth, float startArcAngle, float directionTolerance)
{}


void CActorTemplate::CancelExactPositioningAnimation()
{}


void CActorTemplate::PlayAnimation(const char* sAnimationName, bool bSignal)
{}


void CActorTemplate::EnableTimeDemo(bool bTimeDemo)
{}


void CActorTemplate::SwitchDemoModeSpectator(bool activate)
{}


IVehicle* CActorTemplate::GetLinkedVehicle() const
{
	return nullptr;
}


void CActorTemplate::OnAIProxyEnabled(bool enabled)
{}


void CActorTemplate::OnReturnedToPool()
{}


void CActorTemplate::OnPreparedFromPool()
{}


bool CActorTemplate::ShouldMuteWeaponSoundStimulus() const
{
	return false;
}


void CActorTemplate::OnReused(IEntity *pEntity, SEntitySpawnParams &params)
{}


void CActorTemplate::RegisterForGOEvents()
{
	// TODO: Figure out which events you want to register for.

	// Lists the game object events we want to be notified about.
	//const int EventsToRegister [] =
	//{
	//	eGFE_OnCollision,			// Collision events.
	//	eGFE_BecomeLocalPlayer,		// Become client actor events.
	//};

	//// Register for the specified game object events.
	//GetGameObject ()->RegisterExtForEvents (this, EventsToRegister, sizeof (EventsToRegister) / sizeof (int));
}
