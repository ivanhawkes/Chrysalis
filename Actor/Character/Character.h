#pragma once

#include <IAnimatedCharacter.h>
#include <IActorSystem.h>
#include <StateMachine/StateMachine.h>
#include <Actor/Character/ICharacterEventListener.h>
#include <Actor/Character/Movement/CharacterRotation.h>
#include <Actor/ActorState.h>
#include <Actor/ActorPhysics.h>
#include <Actor/ActorStance.h>
#include <CryAISystem/IAgent.h>


class CPlayer;
class IActionController;
class CCharacterMovementController;
struct SAnimationContext;
struct SActorMovementRequest;
struct IEntityLocking;


enum ELadderLeaveLocation
{
	eLLL_First = 0,
	eLLL_Top = eLLL_First,
	eLLL_Bottom,
	eLLL_Drop,
	eLLL_Count,
};


/**
An implementation of the IActor interface. A CCharacter is an actor that represents a character within the game,
either an NPC or PC which can be controlled by a player.

Characters may have inventory.

\sa	CGameObjectExtensionHelper&lt;CCharacter, IActor&gt;
*/

// TODO: probably needs to also implement IInventoryListener to listen for inventory changes.

class CCharacter : public CGameObjectExtensionHelper < CCharacter, IActor >
{
	//friend class ccc;

private:
	// Declaration of the state machine that controls character movement.
	DECLARE_STATE_MACHINE(CCharacter, Movement);

public:

	/** This instance's default constructor. */
	CCharacter();


	/** This instance's default destructor. */
	virtual ~CCharacter();


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

	// It is critical we override the event priority to ensure we handle the event before CAnimatedCharacter.
	virtual IComponent::ComponentEventPriority GetEventPriority(const int eventID) const override;

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
	\param	bUsingAIIgnoreCharacter	(Optional) Specifies whether the specified AI is ignoring this instance.

	\return	True If the specified AI is a friend of this instance. False otherwise.
	*/
	virtual bool IsFriendlyEntity(EntityId entityId, bool bUsingAIIgnoreCharacter = true) const;


	/**
	Gets this instance's local-space eye position (for a human, this is typically Vec3 (0, 0, 1.82f)). If only one eye is
	available (left_eye, right_eye) then that is used as the local position. In the case of two eyes, the position is
	the average of the two eyes.

	Position is calculated each time using the attachment manager, so it will be better to cache the results if you
	need to call this a few times in an update.

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

	\return The currently holstered item of this instance.
	**/
	virtual IItem* GetHolsteredItem() const;


	/**
	Holsters or unholsters the specified item.

	\param	holster			  	Specifies whether to holster the specified item or not. True to
								holster, false to unholster.
	\param	playSelect		  	Specifies whether to play the "select" animation or not (this is
								typically a holstering animation or grabbing for the specified item
								animation).
	\param	selectSpeedBias   	The selection speed biased. used to control how fast the holstering /
								unholstering animation is.
	\param	hideLeftHandObject	Specifies whether to hide the item in the other hand while holstering
								/ unholstering or not.
	**/
	virtual void HolsterItem(bool holster, bool playSelect = true, float selectSpeedBias = 1.0f, bool hideLeftHandObject = true);


	/**
	Gets current item identifier.

	\param	includeVehicle true to include, false to exclude the vehicles in the search.

	\return The current item identifier.
	**/
	EntityId GetCurrentItemId(bool includeVehicle = false) const;


	/**
	Gets current item.

	\param	includeVehicle true to include, false to exclude the vehicles in the search.

	\return null if it fails, else the current item.

	\sa		GetCurrentItemId for obtaining the Id, since this is a more direct call into the item system.
	**/
	virtual IItem* GetCurrentItem(bool includeVehicle = false) const;


	/**
	Drop the specified item to the ground.

	\param	itemId		 The EntityId of the item to drop.
	\param	impulseScale The amount of impulse to apply to the dropped item.
	\param	selectNext   Specifies whether to select the next item.
	\param	byDeath		 Specifies whether this drop was the result of this instance being killed.

	\return True if the specified item was dropped. False otherwise.
	**/
	virtual bool DropItem(EntityId itemId, float impulseScale = 1.0f, bool selectNext = true, bool byDeath = false);


	/**
	Gets this instance's inventory.

	\return This instance's inventory.
	**/
	virtual IInventory* GetInventory() const;


	/**
	Notifies this instance's entity script? That this instance's current item has changed.

	\param [in,out]	newItem The newly equipped item.
	**/
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
	Query if this object is attached to a player. This does not have to be the local player, just as long as a player
	is attached.

	WARNING! Older code might try and cast this object to a player after receiving a true result here. Need to weed
	that code out and replace with another method.

	\return	true if player, false if not.
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
	Gets the name of this instance's actor class (e.g. "CCharacter").

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


	/**
	***
	*** ICharacterEventListener
	***

	\param [in,out]	pActor	If non-null, the actor.
	\param	bIsGod		  	true if this object is god.
	*/
	virtual void OnDeath(IActor* pActor, bool bIsGod);


	/**
	Executes the enter vehicle action.

	\param [in,out]	pActor	   	If non-null, the actor.
	\param	strVehicleClassName	Name of the vehicle class.
	\param	strSeatName		   	Name of the seat.
	\param	bThirdPerson	   	true to third person.
	*/
	virtual void OnEnterVehicle(IActor* pActor, const char* strVehicleClassName, const char* strSeatName, bool bThirdPerson);


	/**
	Executes the exit vehicle action.

	\param [in,out]	pActor	If non-null, the actor.
	*/
	virtual void OnExitVehicle(IActor* pActor);


	/**
	Executes the health changed action.

	\param [in,out]	pActor	If non-null, the actor.
	\param	newHealth	  	The new health.
	*/
	virtual void OnHealthChanged(IActor* pActor, float newHealth);


	/**
	Executes the item dropped action.

	\param [in,out]	pActor	If non-null, the actor.
	\param	itemId		  	Identifier for the item.
	*/
	virtual void OnItemDropped(IActor* pActor, EntityId itemId);


	/**
	Executes the item picked up action.

	\param [in,out]	pActor	If non-null, the actor.
	\param	itemId		  	Identifier for the item.
	*/
	virtual void OnItemPickedUp(IActor* pActor, EntityId itemId);


	/**
	Executes the item used action.

	\param [in,out]	pActor	If non-null, the actor.
	\param	itemId		  	Identifier for the item.
	*/
	virtual void OnItemUsed(IActor* pActor, EntityId itemId);


	/**
	Executes the object grabbed action.

	\param [in,out]	pActor	If non-null, the actor.
	\param	bIsGrab		  	true if this object is grab.
	\param	objectId	  	Identifier for the object.
	\param	bIsNPC		  	true if this object is NPC.
	\param	bIsTwoHanded  	true if this object is two handed.
	*/
	virtual void OnObjectGrabbed(IActor* pActor, bool bIsGrab, EntityId objectId, bool bIsNPC, bool bIsTwoHanded);


	/**
	Executes the picked up pickable ammo action.

	\param [in,out]	pActor   	If non-null, the actor.
	\param [in,out]	pAmmoType	If non-null, type of the ammo.
	\param	count			 	Number of.
	*/
	virtual void OnPickedUpPickableAmmo(IActor* pActor, IEntityClass* pAmmoType, int count);


	/**
	Executes the revive action.

	\param [in,out]	pActor	If non-null, the actor.
	\param	bIsGod		  	true if this object is god.
	*/
	virtual void OnRevive(IActor* pActor, bool bIsGod);


	/**
	Executes the spectator mode changed action.

	\param [in,out]	pActor	If non-null, the actor.
	\param	mode		  	The mode.
	*/
	virtual void OnSpectatorModeChanged(IActor* pActor, uint8 mode);


	/**
	Executes the special move action.

	\param [in,out]	pActor	If non-null, the actor.
	\param	move		  	The move.
	*/
	virtual void OnSpecialMove(IActor* pActor, ICharacterEventListener::ESpecialMove move);


	/**
	Executes the sprint stamina changed action.

	\param [in,out]	pActor	If non-null, the actor.
	\param	newStamina	  	The new stamina.
	*/
	virtual void OnSprintStaminaChanged(IActor* pActor, float newStamina);


	/**
	Executes the stance changed action.

	\param [in,out]	pActor	If non-null, the actor.
	\param	stance		  	The stance.
	*/
	virtual void OnStanceChanged(IActor* pActor, EStance stance);


	/**
	Executes the toggle third person action.

	\param [in,out]	pActor	If non-null, the actor.
	\param	bThirdPerson  	true to third person.
	*/
	virtual void OnToggleThirdPerson(IActor* pActor, bool bThirdPerson);


	// ***
	// *** CCharacter
	// ***

public:

	/**
	Gets actor statistics.

	\return	null if it fails, else the actor statistics.
	*/
	SActorState *GetActorStats() { return &m_stats; };


	/**
	Gets actor statistics.

	\return	null if it fails, else the actor statistics.
	*/
	const SActorState *GetActorStats() const { return &m_stats; };


	/**
	Gets actor physics.

	\return	null if it fails, else the actor physics.
	*/
	SActorPhysics *GetActorPhysics() { return &m_actorPhysics; };


	/**
	Gets actor physics.

	\return	null if it fails, else the actor physics.
	*/
	const SActorPhysics *GetActorPhysics() const { return &m_actorPhysics; };


	/**
	Provides access to our movement request object.
	
	This is used extensively by the movement state machine to store state as it's calculated / updated. Some states will
	make changes to the move request object e.g. flying and ladders. Most changes are enacted by the ground movement
	state, as you would generally expect. It's final state will be fed into the FinalizeMovementRequest function to pass
	the requested movement into the animation system.
	
	\return The move request.
	**/
	ILINE SCharacterMoveRequest& GetMoveRequest() { return m_moveRequest; }

	
	ILINE CCharacterRotation& GetCharacterRotation() { return *m_characterRotation; }


	/**
	Changes the seed value of fate for a character. All fate based tests after this point in time
	will be affected and based off this new seed.

	\param	fate	The fate.
	*/
	ILINE void ChangeYourFate() { m_fate = (static_cast<uint64_t>(cry_random_uint32()) << 32) + static_cast<uint64_t>(cry_random_uint32()); }


	/**
	Sets a fate. Intended for use in games where they don't want to make use of a randomised fate.

	\param	fate	The fate.
	*/
	ILINE void SetFate(uint64 fate) { m_fate = fate; }

	/**
	Gets the character's complete fate.

	\return	The fate.
	*/
	ILINE uint64 GetFate() { return m_fate; }


	/**
	Gets the character's greater fate. This portion of the fate seed is intended for large scale events
	in the character's life.

	\return	The greater fate.
	*/
	ILINE uint32 GetGreaterFate() { return static_cast<uint32_t>(m_fate >> 32); }


	/**
	Gets the character's lesser fate. This portion of the fate seed is intended for small scale events
	in the character's life.

	\return	The lesser fate.
	*/
	ILINE uint32 GetLesserFate() { return static_cast<uint32_t>(m_fate & 0x0000FFFF); }


private:

	/**
	Physicalies this instance.

	\return	True if physicalization was successful. False otherwise.
	*/
	bool Physicalize();


	/**
	Pre physics update.
	*/
	void PrePhysicsUpdate();


	/**
	Updates to the animation state described by frameMovementParams.

	\param	frameMovementParams	A variable-length parameters list containing frame movement parameters.
	*/
	void UpdateAnimationState(const SActorMovementRequest& movementRequest);


	/**
	Processes this instance's movement. Used to check if this instance should move. If this instance should move
	then movement will be performed.

	This method should be called every frame.

	\param	DeltaTime	The amount of time (in seconds) that have elapsed since last frame.
	*/
	void inline ProcessMovement(float DeltaTime);


	/**
	Processes this instance's rotation. Used to check if this instance should rotate. If this instance should
	rotate then rotation will be performed.

	This method should be called every frame.

	\param	DeltaTime	The amount of time (in seconds) that have elapsed since last frame.
	*/
	void inline ProcessRotation(float DeltaTime);


	/**
	Registers this instance for GameObject event notifications (will receive HandleEvent() calls).
	*/
	void RegisterEvents();


	/**
	Select movement hierarchy for our HSM.
	*/
	void SelectMovementHierarchy();


	/**
	Scripts are able to raise an event which is passed back to the c++ code though the ENTITY_EVENT_SCRIPT_EVENT.
	This method handles those events.

	\param	eventName	  	Name of the event.
	\param	eventValueType	Type of the event value.
	\param	pEventValue   	The event value.
	*/
	void OnScriptEvent(SEntityEvent& event);


	/** Whenever a property in the editor is changed, this function is called. */
	void OnEditorPropertyChanged();


	/** Called to indicate the entity must reset itself. This is often done during PostInit() and
	additionally by the editor when you both enter and leave game mode. */
	void OnReset();


	/** Specifies whether this instance is the client actor. */
	bool m_bClient = false;

	/** True if this actor is using a third person camera. */
	bool m_bIsThirdPerson = false;

	/** The player's inventory. */
	IInventory* m_pInventory = nullptr;

	/** The player's animated character. */
	IAnimatedCharacter* m_pAnimatedCharacter = nullptr;

	/** The action controller. */
	IActionController* m_pActionController = nullptr;

	/** Context for the animation. */
	SAnimationContext* m_pAnimationContext = nullptr;

	/** The movement controller. */
	CCharacterMovementController* m_pMovementController = nullptr;

	/** Identifier for the team. */
	int m_teamId = 0;

	/**
	If a player is controlling this character, this pointer will be valid.
	*/
	CPlayer* m_pAttachedPlayer = nullptr;

	/** The current state for a character. This is shared by a lot of the state machine code. */
	SActorState m_stats;

	/** The actor physics. */
	SActorPhysics m_actorPhysics;

	/** The move request keeps track of how we wish to move this character based on input, state machine, and movement controllers. */
	SCharacterMoveRequest m_moveRequest {};

	/** A randomly generated seed that is specific to the character and lasts for their entire lifetime. It's intention
	is for use as a seed for pseudo-random outcomes that have been pre-determined at or close to the time of the
	character's creation. Anything attributable to 'Deus Ex Machina' should be derived from this value.

	The value can also be used for simple things like innate aptitude for various subjects. It can act as a simple
	form of DNA, locking in attributes specific to a character.

	Because it's possible to use memory sniffer to determine this value after creation, it's suggested you either
	start with a 0 value "no fate" or a random value that is switched once or more at later stages of a character's
	development. */
	uint64 m_fate = 0;

	/** A class that assists in working out animated character rotation. */
	CCharacterRotation* m_characterRotation;

	// ***
	// *** AI / Player Control
	// ***


public:
	/** true if this object is controlled by an AI. */
	bool m_isAIControlled;

	/**
	Query if this object is controlled by AI. This doesn't imply that there is no player for this character, simply
	that they are under AI control at present.

	\return	true if AI controlled, false if not.
	*/
	ILINE bool IsAIControlled() const { return m_isAIControlled; };


	/**
	Call this routine when a player has attached to this character to complete the circle. The player is now in
	control of this character's movements and may view them through a camera attached to this character. This routine is
	not meant for direct calling, instead use the AttachToCharacter routine that is provided on a valid CPlayer object.

	\param [in,out]	player	The player.
	*/
	void OnPlayerAttach(CPlayer& player);


	/**
	Detach the player from this character. This needs to be called when a player is no longer
	controlling or attached to this character.
	*/
	void OnPlayerDetach();


	// ***
	// *** Character stances
	// ***

public:

	ILINE EStance GetStance() const
	{
		return m_stance.Value();
	}


	ILINE const SActorStance* GetStanceInfo(EStance stance) const
	{
		if (stance < 0 || stance > STANCE_LAST)
			return &m_defaultStance;

		return &m_stances [stance];
	}

	CCoherentValue<EStance> m_stance;
	EStance m_desiredStance;
	static SActorStance m_defaultStance;
	SActorStance m_stances [STANCE_LAST];


	// ***
	// *** Cut-scenes
	// ***
	// *** While cut-scenes are played for the local player, the requests are likely to also be made on behalf
	// *** of characters. In such cases, we pass along the request to the attached player if there is one.
	// ***

public:
	/** A cut-scene has been triggered for this character. */
	void OnBeginCutScene() {};


	/** A cut-scene has finished playing or been cancelled for this character. */
	void OnEndCutScene() {};


	// ***
	// *** Handle interactions with other entities.
	// *** TODO: This code feels a little loose, like it's API needs to be better defined.
	// ***

public:
	/**
	Gets the interactor.

	\return	null if it fails, else the interactor.
	*/
	IEntityLocking* GetInteractor();


	/**
	Locks the interactor.

	\param	lockId	Identifier for the lock.
	\param	lock  	true to lock, false to unlock.
	*/
	void LockInteractor(EntityId lockId);


	/**
	Unlocks the interactor.

	\param	unlockId	Identifier for the unlock.
	*/
	void UnlockInteractor(EntityId unlockId);


	/** Resets the interactor. */
	void ResetInteractor();


	/**
	Action handler for the "use" verb for entities that provide a use function.

	\param	playerId	The entityId for the player who invoked this action.
	*/
	void OnActionItemUse(EntityId playerId);


	/**
	Action handler for picking up an item / entitiy.

	\param	playerId	The entityId for the player who invoked this action.
	*/
	void OnActionItemPickup(EntityId playerId);


	/**
	Action handler for dropping an item / entity.

	\param	playerId	The entityId for the player who invoked this action.
	*/
	void OnActionItemDrop(EntityId playerId);


	/**
	Action handler for throwing an item. General expectation is this will be used for "Pick and Throw" weapons but it
	might need to also handle regular weapons and even ordinary items.

	\param	playerId	The entityId for the player who invoked this action.
	*/
	void OnActionItemThrow(EntityId playerId);

private:
	/**
	Core part of the interactor lock routine. This is split out and private to ensure we can control extra parts
	required for both locking and unlocking.

	\param	lockId	Identifier for the lock.
	\param	lock  	true to lock, false to unlock.
	*/
	void LockInteractor(EntityId lockId, bool lock);


	/** An interactor which is used to handle 'Useable' / 'Used' logic in the world. */
	IEntityLocking* m_pInteractor = nullptr;


	// ***
	// *** Character life-cycle
	// ***

public:
	enum EReasonForRevive
	{
		kRFR_FromInit,
		kRFR_StartSpectating,
		kRFR_Spawn,
		kRFR_ScriptBind,
	};


	/** Resets the character to an initial state. */
	void Reset();


	/** Kill the character. */
	void Kill();


	/**
	Revive the character from death.

	\param	reasonForRevive	the reason for revive.
	*/
	void Revive(EReasonForRevive reasonForRevive = kRFR_Spawn);
};
