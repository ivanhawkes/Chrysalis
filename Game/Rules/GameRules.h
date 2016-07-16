#pragma once

#include "IGameRulesSystem.h"


/**
An implementation of the IGameRules interface. Used to control the "win" conditions of a CRYENGINE game and
provides various helper functions specific to a CRYENGINE Game. Also acts as an interface between a CRYENGINE game
and the rest of the systems (especially the network system). Every CRYENGINE game needs at least one GameRules
implementation.

\sa	CGameObjectExtensionHelper&lt;CGameRules, IGameRules&gt;
*/
class CGameRules : public CGameObjectExtensionHelper < CGameRules, IGameRules >
{
public:

	// ***
	// *** IGameObjectExtension
	// ***

	void GetMemoryUsage(ICrySizer *pSizer) const override;
	bool Init(IGameObject * pGameObject) override;
	void PostInit(IGameObject * pGameObject) override;
	void InitClient(int channelId) override;
	void PostInitClient(int channelId) override;
	bool ReloadExtension(IGameObject * pGameObject, const SEntitySpawnParams &params) override;
	void PostReloadExtension(IGameObject * pGameObject, const SEntitySpawnParams &params) override;
	bool GetEntityPoolSignature(TSerialize signature) override;
	void Release() override;
	void FullSerialize(TSerialize ser) override;
	bool NetSerialize(TSerialize ser, EEntityAspects aspect, uint8 profile, int pflags) override;
	void PostSerialize() override {};
	void SerializeSpawnInfo(TSerialize ser) override {};
	ISerializableInfoPtr GetSpawnInfo() override;
	void Update(SEntityUpdateContext& ctx, int updateSlot) override;
	void HandleEvent(const SGameObjectEvent& event) override;
	void ProcessEvent(SEntityEvent& event) override;
	void SetChannelId(uint16 id) override;
	void SetAuthority(bool auth) override;
	const void* GetRMIBase() const override;
	void PostUpdate(float frameTime) override;
	void PostRemoteSpawn() override;


	// ***
	// *** IGameRules
	// ***


	/**
	Gets whether the disconnecting client should be kept for a few more moments or not.
	
	\param	channelId	The server channel Id of the server that the client is disconnecting from.
	\param	cause	 	The cause.
	\param	desc	 	The description.
	
	\return	Whether the disconnecting client should be kept for a few more moments or not.
	*/
	virtual bool ShouldKeepClient(int channelId, EDisconnectionCause cause, const char *desc) const;


	/** Automatically called after a level has been loaded. Used to pre-cache anything that is required. */
	virtual void PrecacheLevel();


	/**
	Automatically called by various systems to pre-cache needed game resources for the specified level.
	
	\param	resourceName	The name of the resource to pre-cache.
	\param	resourceType	The type of the resource to pre-cache.
	*/
	virtual void PrecacheLevelResource(const char* resourceName, EGameResourceType resourceType);


	/**
	Checks to see whether the XML NodeRef exists in the pre-cache map, keyed by filename. If it does, it returns It.
	if it doesn't, it returns a nullptr ref.
	
	\param	sFilename	The filename of the pre-cache XML file.
	
	\return	The found XML NodeRef if successful. A nullptr NodeRef otherwise.
	*/
	virtual XmlNodeRef FindPrecachedXmlFile(const char *sFilename);


	/**
	Called when the client has connected to the server.
	
	\param [in,out]	pNetChannel	The INetChannel Used For Connection.
	*/
	virtual void OnConnect(struct INetChannel *pNetChannel);


	/**
	Called when the client has disconnected from the server.
	
	\param	cause	The Cause For DisConnection.
	\param	desc 	The DisConnections Description.
	*/
	virtual void OnDisconnect(EDisconnectionCause cause, const char *desc);


	/**
	Called when a client connects to a server.

	\param	channelId	The channel Id of the server that the client is connected to.
	\param	isReset  	Specifies whether this connection was a result of a reset.

	\return	True if the client should stay connected. False otherwise.
	*/
	virtual bool OnClientConnect(int channelId, bool isReset);


	/**
	Called when the server has been disconnected from by a client.

	\param	channelId 	The channel Id of the server that the client is connected to.
	\param	cause	  	The cause for disconnection.
	\param	desc	  	The disconnections description.
	\param	keepClient	Specifies whether to keep the client or not.
	*/
	virtual void OnClientDisconnect(int channelId, EDisconnectionCause cause, const char *desc, bool keepClient);


	/**
	Called when the server detects that a client has entered the current game.

	\param	channelId	The channel Id of the server that the client is connected to.
	\param	isReset  	Specifies whether this connection was a result of a reset.

	\return	True if the client should stay connected. False otherwise.
	*/
	virtual bool OnClientEnteredGame(int channelId, bool isReset);


	/**
	Called when an entity has been spawned.

	\param [in,out]	pEntity	The Entity That Was Spawned.
	*/
	virtual void OnEntitySpawn(IEntity *pEntity);


	/**
	Called when an entity has been removed.

	\param [in,out]	pEntity	the entity that was removed.
	*/
	virtual void OnEntityRemoved(IEntity *pEntity);


	/**
	Called when an entity has been reused.

	\param [in,out]	pEntity	The entity that has been reused.
	\param	params		   	The parameters that were used to re-spawn the specified entity.
	\param	prevId		   	The specified entity's previous EntityId.
	*/
	virtual void OnEntityReused(IEntity *pEntity, SEntitySpawnParams &params, EntityId prevId);


	/**
	Sends the specified text message to other clients using the specified settings.
	
	\param	type	 	The type of message to send (eg. console, announcement, error).
	\param	msg		 	The message you want to send.
	\param	to		 	The clients to send the message to (eg. all, remote, local).
	\param	channelId	The channel Id to send to. only valid if you are also using The eRMI_ToClientChannel Text Message
						Type. Indicates that you want the message to only be sent to everyone in the specified channel.
	\param	p0		 	The first message-specific arbitrary string value to send with the message.
	\param	p1		 	The second message-specific arbitrary string value to send with the message.
	\param	p2		 	The third message-specific arbitrary string value to send with the message.
	\param	p3		 	The fourth message-specific arbitrary string value to send with the message.
	*/
	virtual void SendTextMessage(ETextMessageType type, const char *msg, uint32 to = eRMI_ToAllClients, int channelId = -1,
		const char *p0 = 0, const char *p1 = 0, const char *p2 = 0, const char *p3 = 0);


	/**
	Sends the specified chat message using the specified settings.
	
	\param	type		The type of message to send.
	\param	sourceId	The EntityId of the entity that is sending the specified message.
	\param	targetId	The EntityId of the entity that is to receive the specified message. Only valid if you are also
						using the eChatToTarget chat message type.
	\param	msg			The message you want to send.
	*/
	virtual void SendChatMessage(EChatMessageType type, EntityId sourceId, EntityId targetId, const char *msg);


	/**
	Called to perform client-side handling of a detected hit.

	\param	hitInfo	The hit information describing the hit.
	*/
	virtual void ClientHit(const HitInfo &hitInfo);


	/**
	Called to perform server-side handling of a detected hit.

	\param	hitInfo	The hit information describing the hit.
	*/
	virtual void ServerHit(const HitInfo &hitInfo);


	/**
	Gets the Id of the specified hittype.

	\param	crc	The CRC of the HitType you want to get the Id of.

	\return	The Id of the specified HitType.
	*/
	virtual int GetHitTypeId(const uint32 crc) const;


	/**
	Gets the id Of the specified HitType.

	\param	type	The name of the HitType you want to get the Id of.

	\return	The Id of the specified HitType.
	*/
	virtual int GetHitTypeId(const char *type) const;


	/**
	Gets the name of the specified HitType.
	
	\param	id	The Id of the HitType you want to get the name of.
	
	\return	The name of the specified HitType.
	*/
	virtual const char *GetHitType(int id) const;


	/**
	Called when a vehicle has been destroyed.
	
	\param	id	The EntityId of the vehicle that has been destroyed.
	*/
	virtual void OnVehicleDestroyed(EntityId id);


	/**
	Called when a vehicle has been submerged.
	
	\param	id   	The EntityId of the vehicle that has been submerged.
	\param	ratio	The amount (in percentage) that the vehicle is submerged. This value is in the range of 0.0 - 1.0.
	*/
	virtual void OnVehicleSubmerged(EntityId id, float ratio);


	/**
	Gets whether the specified entity should be able to enter a vehicle.
	
	\param	playerId	The EntityId of the entity to check whether it can enter a vehicle.
	
	\return	Whether the specified entity should be able to enter a vehicle.
	*/
	virtual bool CanEnterVehicle(EntityId playerId);


	/**
	Prepares the specified entity to be allowed to re-spawn.

	\param	entityId	The EntityId of the entity to prepare for re-spawn.
	*/
	virtual void CreateEntityRespawnData(EntityId entityId);


	/**
	Gets whether the specified entity has re-spawn data.
	
	\param	entityId	The EntityId of the entity to check for the existence of re-spawn data.
	
	\return	Whether the specified entity has re-spawn data.
	*/
	virtual bool HasEntityRespawnData(EntityId entityId) const;


	/**
	Schedules the specified entity to be re-spawned in the specified time.
	
	\param	entityId	The entityid of the entity to schedule for re-spawn.
	\param	unique  	Specifies whether this entity should be spawned if another entity with the same Id already
						exists. True to block this entity from spawning if the same entity already exists.
	\param	timer   	The amount of time (in seconds) to wait before spawning the specified entity.
	*/
	virtual void ScheduleEntityRespawn(EntityId entityId, bool unique, float timer);


	/**
	Aborts The scheduled re-spawn of the specified entity.
	
	\param	entityId   	The EntityId of the entity to abort re-spawning of.
	\param	destroyData	Specifies whether to also delete the stored re-spawn data for the specified entity.
	*/
	virtual void AbortEntityRespawn(EntityId entityId, bool destroyData);


	/**
	Schedules the specified entity to be removed in the specified time.
	
	\param	entityId  	The EntityId of the entity to schedule for removal.
	\param	timer	  	The amount of time (in seconds) to wait before removing the specified entity.
	\param	visibility	Specifies whether to reset the timer at the time of removal if it is in view of a player.
	*/
	virtual void ScheduleEntityRemoval(EntityId entityId, float timer, bool visibility);


	/**
	Aborts the scheduled removal of the specified entity.

	\param	entityId	The EntityId of the entity to abort removal of.
	*/
	virtual void AbortEntityRemoval(EntityId entityId);


	/**
	Adds the specified IHitListener. An added IHitListener will be notified about a detected hit.
	
	\param [in,out]	pHitListener	The IHitListener To Add.
	*/
	virtual void AddHitListener(IHitListener* pHitListener);


	/**
	Removes the specified IHitListener. A removed IHitListener will no longer be notified about a detected hit.

	\param [in,out]	pHitListener	The IHitListener to remove.
	*/
	virtual void RemoveHitListener(IHitListener* pHitListener);


	/**
	Called when two entities collide. This method should also dispatch this event to the scripts.
	
	\param	event	The information describing the detected collision.
	
	\return	True if the collision event was handled. False otherwise.
	*/
	virtual bool OnCollision(const SGameCollision& event);


	/**
	Called when two entities collide. Used to determine if AI should receive stimulus.
	
	\param	pEvent	The information describing the detected collision.
	*/
	virtual void OnCollision_NotifyAI(const EventPhys * pEvent);


	/**
	Used to allow this instance to extend the 'status' command.
	*/
	virtual void ShowStatus();


	/**
	Gets whether this instance is time limited or not (whether the game is time limited).

	\return	Whether this instance is time limited or not.
	*/
	virtual bool IsTimeLimited() const;


	/**
	Gets the remaining time (in seconds) of this instance (the amount of time (in seconds) left of the game).

	\return	The remaining time (in seconds) of this instance.
	*/
	virtual float GetRemainingGameTime() const;


	/**
	Sets the remaining time (in seconds) of this instance (the amount of time (in seconds) left of the game).
	
	\param	seconds	The amount of time (in seconds) left of the game.
	*/
	virtual void SetRemainingGameTime(float seconds);


	/**
	Clears all migrating players' details.
	*/
	virtual void ClearAllMigratingPlayers(void);


	/**
	Sets the game channel for the specified migrating player.
	
	\param	name	 	The name of the migrating player you are setting the channel of.
	\param	channelID	The channel Id to assign the specified migrating player to.
	
	\return	The specified players' EntityId.
	*/
	virtual EntityId SetChannelForMigratingPlayer(const char* name, uint16 channelID);


	/**
	Stores the details of the specified migrating player.

	\param [in,out]	pActor	The migrating player to store the details Of.
	*/
	virtual void StoreMigratingPlayer(IActor* pActor);


	/**
	Gets the name of the team.

	\param	teamId	Identifier for the team.

	\return	null if it fails, else the team name.
	*/
	const char *GetTeamName(int teamId) const { return "No team"; };

private:

	/**
	The channel Id of this instance.
	*/
	uint16 m_ChannelID;


	// ***
	// *** CGameRules
	// ***

public:

	/**
	This Instance's Default Constructor.
	*/
	CGameRules();


	/**
	This Instance's Default Destructor.
	*/
	~CGameRules();


};