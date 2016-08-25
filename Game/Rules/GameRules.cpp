#include <StdAfx.h>

#include "GameRules.h"
#include "IActorSystem.h"
#include <Game/Game.h>
#include <Actor/Player/Player.h>


// ***
// *** IGameObjectExtension
// ***

void CGameRules::GetMemoryUsage(ICrySizer *pSizer) const
{
	pSizer->Add(*this);
}


bool CGameRules::Init(IGameObject * pGameObject)
{
	// Stores the specified IGameObject in this instance.
	SetGameObject(pGameObject);

	// Add this instance to the network.
	if (!pGameObject->BindToNetwork())
		return false;// Failed To add this instance to the network.

	// Sets this instance as the currently active game rules.
	gEnv->pGame->GetIGameFramework()->GetIGameRulesSystem()->SetCurrentGameRules(this);

	// Initialization was successful.
	return true;
}


void CGameRules::PostInit(IGameObject * pGameObject)
{
	// Allow this instance to be updated every frame.
	pGameObject->EnableUpdateSlot(this, 0);

	// Allow this instance to be post-updated every frame.
	pGameObject->EnablePostUpdates(this);
}


void CGameRules::InitClient(int channelId)
{}


void CGameRules::PostInitClient(int channelId)
{}


bool CGameRules::ReloadExtension(IGameObject * pGameObject, const SEntitySpawnParams &params)
{
	// Restores the specified IGameObject in this instance.
	ResetGameObject();

	CRY_ASSERT_MESSAGE(false, "CGameRules::ReloadExtension Not Implemented");

	return true;
}


void CGameRules::PostReloadExtension(IGameObject * pGameObject, const SEntitySpawnParams &params)
{}


bool CGameRules::GetEntityPoolSignature(TSerialize signature)
{
	return true;
}


void CGameRules::Release()
{
	// Destroy this instance.
	delete this;
}


void CGameRules::FullSerialize(TSerialize ser)
{}


bool CGameRules::NetSerialize(TSerialize ser, EEntityAspects aspect, uint8 profile, int pflags)
{
	return true;
}


ISerializableInfoPtr CGameRules::GetSpawnInfo()
{
	return nullptr;
}


void CGameRules::Update(SEntityUpdateContext& ctx, int updateSlot)
{}


void CGameRules::HandleEvent(const SGameObjectEvent& event)
{}


void CGameRules::ProcessEvent(SEntityEvent& event)
{}


void CGameRules::SetChannelId(uint16 id)
{
	// Store this instance's new channel ID.
	m_ChannelID = id;
}


void CGameRules::SetAuthority(bool auth)
{}


const void* CGameRules::GetRMIBase() const
{
	return CGameObjectExtensionHelper::GetRMIBase();
}


void CGameRules::PostUpdate(float frameTime)
{}


void CGameRules::PostRemoteSpawn()
{}


// ***
// *** CGameRules
// ***


CGameRules::CGameRules() :
	m_ChannelID(0)
{}


CGameRules::~CGameRules()
{
	// Very important that this gets called. Sets the active game rules to NULL.
	gEnv->pGame->GetIGameFramework()->GetIGameRulesSystem()->SetCurrentGameRules(nullptr);
}


bool CGameRules::ShouldKeepClient(int channelId, EDisconnectionCause cause, const char *desc) const
{
	// Disconnect client immediately.
	return false;
}


void CGameRules::PrecacheLevel()
{}


void CGameRules::PrecacheLevelResource(const char* resourceName, EGameResourceType resourceType)
{}


XmlNodeRef CGameRules::FindPrecachedXmlFile(const char *sFilename)
{
	return nullptr;
}


void CGameRules::OnConnect(struct INetChannel *pNetChannel)
{}


void CGameRules::OnDisconnect(EDisconnectionCause cause, const char *desc)
{}


bool CGameRules::OnClientConnect(int channelId, bool isReset)
{
	auto *pActorSystem = gEnv->pGame->GetIGameFramework()->GetIActorSystem();

	// Called when a new client connects to the server. We need to spawn a player class to get the ball rolling.
	if (pActorSystem->CreateActor(channelId, "PlayerProxy", "Player", ZERO, IDENTITY, Vec3(1, 1, 1)) != nullptr)
		return true;

	return false;
}


void CGameRules::OnClientDisconnect(int channelId, EDisconnectionCause cause, const char *desc, bool keepClient)
{}


bool CGameRules::OnClientEnteredGame(int channelId, bool isReset)
{
	static_cast <CGame*> (gEnv->pGame)->OnClientEnteredGame(gEnv->pGame->GetIGameFramework()->GetIActorSystem()->GetActorByChannelId(channelId));

	return true;
}


void CGameRules::OnEntitySpawn(IEntity *pEntity)
{}


void CGameRules::OnEntityRemoved(IEntity *pEntity)
{}


void CGameRules::OnEntityReused(IEntity *pEntity, SEntitySpawnParams &params, EntityId prevId)
{}


void CGameRules::SendTextMessage(ETextMessageType type, const char *msg, uint32 to, int channelId, const char *p0, const char *p1, const char *p2, const char *p3)
{}


void CGameRules::SendChatMessage(EChatMessageType type, EntityId sourceId, EntityId targetId, const char *msg)
{}


void CGameRules::ClientHit(const HitInfo &hitInfo)
{}


void CGameRules::ServerHit(const HitInfo &hitInfo)
{}


int CGameRules::GetHitTypeId(const uint32 crc) const
{
	return 0;
}


int CGameRules::GetHitTypeId(const char *type) const
{
	return 0;
}


const char *CGameRules::GetHitType(int id) const
{
	return "";
}


void CGameRules::OnVehicleDestroyed(EntityId id)
{}


void CGameRules::OnVehicleSubmerged(EntityId id, float ratio)
{}


bool CGameRules::CanEnterVehicle(EntityId playerId)
{
	return false;
}


void CGameRules::CreateEntityRespawnData(EntityId entityId)
{}


bool CGameRules::HasEntityRespawnData(EntityId entityId) const
{
	return false;
}


void CGameRules::ScheduleEntityRespawn(EntityId entityId, bool unique, float timer)
{}


void CGameRules::AbortEntityRespawn(EntityId entityId, bool destroyData)
{}


void CGameRules::ScheduleEntityRemoval(EntityId entityId, float timer, bool visibility)
{}


void CGameRules::AbortEntityRemoval(EntityId entityId)
{}


void CGameRules::AddHitListener(IHitListener* pHitListener)
{}


void CGameRules::RemoveHitListener(IHitListener* pHitListener)
{}


bool CGameRules::OnCollision(const SGameCollision& event)
{
	// TODO: Apparently we are meant to dispatch this to the scripts if needed.
	// 
	return true;
}


void CGameRules::OnCollision_NotifyAI(const EventPhys * pEvent)
{}


void CGameRules::ShowStatus()
{}


bool CGameRules::IsTimeLimited() const
{
	return false;
}


float CGameRules::GetRemainingGameTime() const
{
	return 1;
}


void CGameRules::SetRemainingGameTime(float seconds)
{}


void CGameRules::ClearAllMigratingPlayers(void)
{}


EntityId CGameRules::SetChannelForMigratingPlayer(const char* name, uint16 channelID)
{
	return 0;
}


void CGameRules::StoreMigratingPlayer(IActor* pActor)
{}
