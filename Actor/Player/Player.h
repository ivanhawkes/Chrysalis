/**
\file	d:\Chrysalis\Source\Chrysalis\Actor\Player\Player.h

Provides functionality for a bare bones player entity. For Chrysalis we envision the player as an entity that represents the person
at the keyboard playing the game. The entity is responsible for gathering player input, overall camera management, and attaching to
character entities. It should be fairly lightweight and instead delegate the work out to other components.

In places where CRYENGINE makes implicit assumptions about the player class also being the character in play, we will proxy as best
as possible to the character class.

At present, entering game mode without being attached to an existing character entity will likely crash or cause unwanted effects.
Code should be added to armour against this.
*/
#pragma once

#include <IActorSystem.h>

struct ICamera;
struct ICameraManager;
struct IPlayerInput;
class CCharacter;
class CItem;
class CWeapon;


/**
An implementation of the IActor interface. Defines the player class. Used to provide the player a way to control
a character in the game. This class provides heath / armour functionality, player camera / movement functionality and any
other functionality needed for a fully playable character. Every CRYENGINE game NEEDS to have an IActor
implementation (e.g. "Player" class (client actor)).

\sa	CGameObjectExtensionHelper&lt;CPlayer, IActor&gt;
*/
class CPlayer : public CGameObjectExtensionHelper <CPlayer, IActor>
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
	void PostUpdate(float frameTime) override;
	void PostRemoteSpawn() override;


	// *** 
	// *** IActor
	// *** 


	/**
	Sets this instance's current health.

	\param	health	The health you want this instance to have.
	*/
	virtual void SetHealth(float health);


	/**
	Gets the current health of this instance.

	\return	The current health of this instance.
	*/
	virtual float GetHealth() const;


	/**
	Gets the current health as a rounded percentage.

	\return	The current health as a rounded percentage.
	*/
	virtual int	GetHealthAsRoundedPercentage() const;


	/**
	Sets the maximum amount of health this instance can have.

	\param	maxHealth	The maximum health you want this instance to have.
	*/
	virtual void SetMaxHealth(float maxHealth);


	/**
	Gets the maximum amount of health this instance can have.

	\return	The maximum amount of health this instance can have.
	*/
	virtual float GetMaxHealth() const;


	/**
	Gets the current amount of armour this instance has.

	\return	The current amount of armour this instance has.
	*/
	virtual int	GetArmor() const;


	/**
	Gets the maximum amount of armour this instance can have.

	\return	The maximum amount of armour this instance can have.
	*/
	virtual int	GetMaxArmor() const;


	/**
	Gets team identifier.

	\return	The team identifier.
	*/
	virtual int GetTeamId() const { return m_teamId; };


	/**
	Gets whether this instance has fallen and needs revival.

	\return	Whether this instance has fallen and needs revival.
	*/
	virtual bool IsFallen() const;


	/**
	Gets whether this instance is dead.

	\return	Whether this instance is dead.
	*/
	virtual bool IsDead() const;


	/**
	Gets whether this instance is immune to damage.

	\return	Whether this instance is immune to damage.
	*/
	virtual int	IsGod();


	/**
	Causes this instance to fall down based on the specified impact it has received.

	\param	hitPos	(Optional) the position of impact.
	*/
	virtual void Fall(Vec3 hitPos = Vec3(0, 0, 0));


	/**
	Gets whether this instance is allowed to perform a "landing bob".

	\return	Whether this instance is allowed to perform a "landing bob".
	*/
	virtual bool AllowLandingBob();


	/**
	Play The specified action (animation). this method is DEPRECATED, and not used.

	\param	action   	The action (animation) to play.
	\param	extension	The file extension? of the animation file to load in order to play the specified action
	(animation).
	\param	looping  	(Optional) Specifies whether the specified action (animation) should loop of not.
	*/
	virtual void PlayAction(const char *action, const char *extension, bool looping = false);


	/**
	Gets this instance's current animation graph state.

	An animation graph is used to decide animation logic (what animation should be played and on what condition they
	should be played).

	\return	This instance's current animation graph state.
	*/
	virtual IAnimationGraphState* GetAnimationGraphState();


	/**
	Resets this instance's animation state.
	*/
	virtual void ResetAnimationState();


	/**
	Calls the "ScriptEvent" script function on this instance's entity script. Used to forward responsibility of
	handling this event to this instance's entity script.

	\param	event	The script event that occurred.
	\param	value	The event specific floating-point value used by the specified event.
	\param	str  	(Optional) The event specific string value used by the specified event.
	*/
	virtual void CreateScriptEvent(const char *event, float value, const char *str = NULL);


	/**
	Indicates that this instance should become aggressive towards the specified AI. Should maybe pull out this
	instance's weapon and aim it at the specified AI.

	\param	entityID	The EntityId of the AI to become aggressive towards.

	\return	True If Successful. False otherwise.
	*/
	virtual bool BecomeAggressiveToAgent(EntityId entityID);


	/**
	Sets the facial alertness level of this instance. Should maybe call some sort of facial animation on this
	instance as well.

	\param	alertness	The new facial alertness level of this instance.
	*/
	virtual void SetFacialAlertnessLevel(int alertness);


	/**
	Requests this instance to play the specified facial expression (animation).

	\param	pExpressionName		  	(Optional) The facial expression to play.
	\param [in,out]	sequenceLength	(Optional) When this method returns, holds the length (in seconds) of the started
	facial animation.
	*/
	virtual void RequestFacialExpression(const char* pExpressionName = NULL, f32* sequenceLength = NULL);


	/**
	Pre-caches The specified facial expression.

	\param	pExpressionName	The name of the facial expression to pre-cache.
	*/
	virtual void PrecacheFacialExpression(const char* pExpressionName);


	/**
	Gets the AI that is currently held by this instance.

	\return	The AI that is currently held by this instance.
	*/
	virtual EntityId GetGrabbedEntityId() const;


	/**
	Hides or shows all of this instance's attachments (typically weapons, but not necessarily).

	\param	isHiding	Specifies whether the attachments should be hidden or not. True to hide, false to show.
	*/
	virtual void HideAllAttachments(bool isHiding);


	/**
	Sets the position of the specified limb using IK.

	\param	pLimbName	The name of the limb to set the position of.
	\param	goalPos  	The position to set the specified limb.
	\param	priority 	The priority to used while setting the specified limb's position.
	*/
	virtual void SetIKPos(const char *pLimbName, const Vec3& goalPos, int priority);


	/**
	Sets the rotation of this instance's vehicle view.

	\param	viewRotation	The rotation to set the vehicle view to.
	*/
	virtual void SetViewInVehicle(Quat viewRotation);


	/**
	Sets The rotation of this instance's view.

	\param	rotation	The rotation to set the view to.
	*/
	virtual void SetViewRotation(const Quat &rotation);


	/**
	Gets the rotation of this instance's view.

	\return	The rotation of this instance's view.
	*/
	virtual Quat GetViewRotation() const;


	/**
	Gets whether the specified AI is a friend of this instance or not.

	\param	entityId				The EntityId of the AI you want to check for friendliness.
	\param	bUsingAIIgnorePlayer	(Optional) Specifies whether the specified AI is ignoring this instance (the player).

	\return	True If the specified AI is a friend of this instance. False otherwise.
	*/
	virtual bool IsFriendlyEntity(EntityId entityId, bool bUsingAIIgnorePlayer = true) const;


	/**
	Gets this instance's local-space eye position (for a human, this is typically Vec3 (0, 0, 1.82f)).

	\return	This instance's local-space eye position.
	*/
	virtual Vec3 GetLocalEyePos() const;


	/**
	Performs a camera shake on this instance's view using the specified parameters.

	\param	angle	 	The angle (in degrees) of the shake (the tilting movement of the shake).
	\param	shift	 	The shift of the shake (the strafing movement of the shave).
	\param	duration 	The duration (in seconds) of the shake.
	\param	frequency	The frequency on the shake (how rapidly it shakes).
	\param	pos		 	The position of the camera at the time of the shake request.
	\param	ID		 	The ID of the shake.
	\param	source   	(Optional) The arbitrary source of the shake (typically the method that invoked the shake).
	*/
	virtual void CameraShake(float angle, float shift, float duration, float frequency, Vec3 pos, int ID, const char* source = "");


	/**
	Gets the currently holstered item of this instance.

	\return	The currently holstered item of this instance.
	*/
	virtual IItem* GetHolsteredItem() const;


	/**
	Holsters or unholsters the specified item.

	\param	holster			  	Specifies whether to holster the specified item or not. True to holster, false to
	unholster.
	\param	playSelect		  	(Optional) Specifies whether to play the "select" animation or not (this is typically a
	holstering animation or grabbing for the specified item animation).
	\param	selectSpeedBias   	(Optional) The selection speed biased. used to control how fast the
	holstering / unholstering animation is.
	\param	hideLeftHandObject	(Optional) Specifies whether to hide the item in the other hand while
	holstering / unholstering or not.
	*/
	virtual void HolsterItem(bool holster, bool playSelect = true, float selectSpeedBias = 1.0f, bool hideLeftHandObject = true);


	/**
	Gets the currently equipped item of this instance.

	\param	includeVehicle	(Optional) Specifies whether this instance's linked vehicle should be included in the search.

	\return	The currently equipped item of this instance.
	*/
	virtual IItem* GetCurrentItem(bool includeVehicle = false) const;


	/**
	Drop the specified item to the ground.

	\param	itemId			The EntityId of the item to drop.
	\param	impulseScale	(Optional) The amount of impulse to apply to the dropped item.
	\param	selectNext  	(Optional) Specifies whether to select the next item.
	\param	byDeath			(Optional) Specifies whether this drop was the result of this instance being killed.

	\return	True if the specified item was dropped. False otherwise.
	*/
	virtual bool DropItem(EntityId itemId, float impulseScale = 1.0f, bool selectNext = true, bool byDeath = false);


	/**
	Gets this instance's inventory.

	\return	This instance's inventory.
	*/
	virtual IInventory* GetInventory() const;


	/**
	Notifies this instance's entity script? That this instance's current item has changed.

	\param [in,out]	newItem	The newly equipped item.
	*/
	virtual void NotifyCurrentItemChanged(IItem* newItem);


	/**
	Gets this instance's movement controller (the class instance that controls how this instance moves around).

	\return	This instance's movement controller.
	*/
	virtual IMovementController* GetMovementController() const;


	/**
	Links this instance to the specified vehicle.

	\param	vehicleId	The EntityId of the vehicle to link this instance to.

	\return	The IEntity of the newly linked vehicle.
	*/
	virtual IEntity* LinkToVehicle(EntityId vehicleId);


	/**
	Gets this instance's currently linked entity.

	\return	This instance's currently linked entity.
	*/
	virtual IEntity* GetLinkedEntity() const;


	/**
	Gets this instance's current spectator mode.

	\return	This instance's current spectator mode.
	*/
	virtual uint8 GetSpectatorMode() const;


	/**
	Gets whether this instance is using third person or not (typically means this instance is using the third person
	model, not the first person arms).

	\return	Whether This Instance Is Using 3RD Person Or Not.
	*/
	virtual bool IsThirdPerson() const;


	/**
	Toggles third person mode of this instance (typically means to toggle between third person and first person models).
	*/
	virtual void ToggleThirdPerson();


	/**
	Gets whether this instance is the "player" class or not.

	\return	Whether this instance is the "player" class or not.
	*/
	virtual bool IsPlayer() const;


	/**
	Gets whether this instance is the "client actor" or not.

	\return	Whether this instance is the "client actor" or not.
	*/
	virtual bool IsClient() const;


	/**
	Gets whether this instance is currently migrating servers or not.

	\return	Whether this instance is currently migrating servers or not.
	*/
	virtual bool IsMigrating() const;


	/**
	Sets whether this instance is migrating servers or not.

	\param	isMigrating	specifies whether this instance is migrating servers or not. True if migrating, false otherwise.
	*/
	virtual void SetMigrating(bool isMigrating);


	/**
	Initializes this instance. Used to initialize the local player (if this instance is the local player).
	*/
	virtual void InitLocalPlayer();


	/**
	Gets the name of this instance's actor class (e.g. "CPlayer").

	\return	The name of this instance's actor class.
	*/
	virtual const char* GetActorClassName() const;


	/**
	Gets this instance's actor class type.

	/sa EACTORCLASSTYPE.

	\return	This instance's actor class type.
	*/
	virtual ActorClass GetActorClass() const;


	/**
	Gets the name of this instance's entity class.

	\return	The name of this instance's entity class.
	*/
	virtual const char* GetEntityClassName() const;


	/**
	Serializes this instance to/from the specified XML.

	\param [in,out]	node	The XML to serialize to/from.
	\param	bLoading		Specifies whether we are serializing to or serializing from the specified XML.
	*/
	virtual void SerializeXML(XmlNodeRef& node, bool bLoading);


	/**
	Serializes this instance between levels.

	\param [in,out]	ser	The serializer to serialize to/from.
	*/
	virtual void SerializeLevelToLevel(TSerialize &ser);


	/**
	Gets the animated character of this instance.

	\return	The animated character of this instance.
	*/
	virtual IAnimatedCharacter* GetAnimatedCharacter();


	/**
	Gets the non-modifiable version of the animated character of this instance.

	\return	The non-modifiable version of the animated character of this instance.
	*/
	virtual const IAnimatedCharacter* GetAnimatedCharacter() const;


	/**
	Plays the specified animation using the specified parameters using exact positioning. (DEPRECATED?)

	\param	sAnimationName	  	The name of the animation you want to play.
	\param	bSignal			  	Specifies whether to notify other AI's about the specified animation?.
	\param	vPosition		  	The position to play the specified animation at.
	\param	vDirection		  	The direction to play the specified animation at.
	\param	startWidth		  	The start width of the specified animation.
	\param	startArcAngle	  	The start arc angle (in degrees) of the specified animation.
	\param	directionTolerance	The direction tolerance of the specified animation.
	*/
	virtual void PlayExactPositioningAnimation(const char* sAnimationName, bool bSignal, const Vec3& vPosition,
		const Vec3& vDirection, float startWidth, float startArcAngle, float directionTolerance);


	/**
	Cancels the currently playing exact positioning animation.
	*/
	virtual void CancelExactPositioningAnimation();


	/**
	Play the specified animation.

	\param	sAnimationName	The name of the animation you want to play.
	\param	bSignal		  	Specifies whether to notify other AI's about the specified animation?.
	*/
	virtual void PlayAnimation(const char* sAnimationName, bool bSignal);


	/**
	Enables "Time Demo" mode (DEPRECATED?).

	\param	bTimeDemo	Specifies whether to enable "Time Demo" mode or not.
	*/
	virtual void EnableTimeDemo(bool bTimeDemo);


	/**
	Switch this instance to be a spectator. Only valid if "Time Demo" mode is also enabled.

	\param	activate	Specifies whether to activate spectator mode or not.
	*/
	virtual void SwitchDemoModeSpectator(bool activate);


	/**
	Gets this instance's currently linked vehicle.

	\return	This instance's currently linked vehicle.
	*/
	virtual IVehicle* GetLinkedVehicle() const;


	/**
	Automatically called by the IAISystem? When this instance's AI Proxy has been enabled or disabled.

	\param	enabled	Specifies whether this instance's AI Proxy was enabled or disabled.
	*/
	virtual void OnAIProxyEnabled(bool enabled);


	/**
	Automatically called by the IEntitySystem? When this instance's entity has been returned to the entity pool.
	*/
	virtual void OnReturnedToPool();


	/**
	Automatically called by the IEntitySystem? When this instance's entity is about to be spawned from the
	entity pool. Used To Prepare This Instance's Entity Before It Is Spawned From The entity pool.
	*/
	virtual void OnPreparedFromPool();


	/**
	Gets whether this instance should not play weapon sounds if it has been stimulated.

	\return	Whether this instance should not play weapon sounds if it has been stimulated.
	*/
	virtual bool ShouldMuteWeaponSoundStimulus() const;


	/**
	Automatically called by the IEntitySystem when this instance's entity has been reused.

	\param [in,out]	pEntity	The entity that has been reused.
	\param	params		   	The parameters that were used to spawn the specified entity.
	*/
	virtual void OnReused(IEntity *pEntity, SEntitySpawnParams &params);


	// ***
	// *** CPlayer
	// ***

	/** This instance's default constructor. */
	CPlayer();


	/** This instance's default destructor. */
	virtual ~CPlayer();


	/**
	Attach to the character with this EntityID. Player will then be in control of that character. The camera will
	switch to following the newly attached entity.

	\param	characterId	Identifier for the character.
	*/
	void AttachToCharacter(EntityId characterId);


	/**
	Gets the player input handler.

	\return	null if it fails, else the player input.
	*/
	IPlayerInput* GetPlayerInput() const { return m_pPlayerInput; }


	/**
	Gets EntityId for the character / actor who is acting as our pawn.

	\return	The character identifier.
	*/
	EntityId GetAttachedEntityId() const;


	/**
	Gets IEntity for the character / actor who is acting as our pawn.

	\return	The character identifier.
	*/
	IEntity* GetAttachedEntity() const;


	/**
	Gets IActor for the character / actor who is acting as our pawn.

	\return	The character identifier.
	*/
	IActor* GetAttachedActor() const;


	/**
	Gets CCharacter for the character / actor who is acting as our pawn.

	\return	The character identifier.
	*/
	CCharacter* GetAttachedCharacter() const;


	/**
	Gets EntityId for actor who is the focus of our camera.

	\return	The character identifier.
	*/
	EntityId GetCameraTargetId() const;


	/**
	Gets camera manager.
	
	\return null if it fails, else the camera manager.
	**/
	ICameraManager* GetCameraManager() const;


	/**
	Gets the player camera currently in use.

	\return	null if it fails, else the current camera.
	*/
	ICamera* GetCamera() const;


	/**
	Gets local player, if there is one for this client.	It is possible there is no local player.

	Convenience function.

	\return	null if it fails, else the local player.
	*/
	ILINE static CPlayer* GetLocalPlayer()
	{
		return static_cast<CPlayer*>(g_pGame->GetIGameFramework()->GetClientActor());
	}


	/**
	Gets the character for the character / actor who is acting as our pawn. It is possible there is no local player
	or they are not presently attached to a character.

	Convenience function.

	\return	null if it fails, else the local character.
	*/
	static CCharacter* GetLocalCharacter();


	/**
	Convenience function to get the local actor, if there is one.

	\return	null if it fails, else the local actor.
	*/
	ILINE static IActor* GetLocalActor()
	{
		return gEnv->pGame->GetIGameFramework()->GetClientActor();
	}


	/**
	If we have a local player and they are attached to a character, this will retrieve the
	item that character is using.

	\return	null if it fails, else the local item.
	*/
	static CItem* GetLocalItem();


	/**
	If we have a local player and they are attached to a character, this will retrieve the
	weapon that character is using.

	\return	null if it fails, else the local weapon.
	*/
	static CWeapon* GetLocalWeapon();


private:

	/** Pre physics update. */
	virtual void PrePhysicsUpdate();


	/**
	Registers this instance for GameObject event notifications (will receive HandleEvent () calls).
	*/
	void RegisterForGOEvents();


	/** Specifies whether this instance is the client actor. */
	bool m_bClient = false;

	/** True if this player is using a third person camera. */
	bool m_bIsThirdPerson = false;

	/** The camera that this instance uses. */
	ICameraManager* m_pCameraManager = nullptr;

	/** The player's inventory. */
	IInventory* m_pInventory = nullptr;

	/** The player's animated character. */
	IAnimatedCharacter* m_pAnimatedCharacter = nullptr;

	/**
	EntityId for the character we are presently attached onto. This is the actor who will receive movement requests
	as well as being the actor to query for position and rotation.
	*/
	EntityId m_attachedCharacterId = INVALID_ENTITYID;

	/** EntityId for the camera target. This is the entity which the player camera is targeting currently. */
	EntityId m_cameraTargetId = INVALID_ENTITYID;

	/** The player input handler. The player is responsible for ensuring the correct character entity / UI gets the input
	as appropriate. */
	IPlayerInput* m_pPlayerInput;

	/** Identifier for the team. */
	int m_teamId = 0;
};