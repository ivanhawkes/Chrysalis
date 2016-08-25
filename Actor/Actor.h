#pragma once

#include <IAnimatedCharacter.h>
#include <IActorSystem.h>
#include <Actor/Fate.h>
#include <Actor/ActorState.h>
#include <Actor/ActorPhysics.h>
#include <Actor/ActorStance.h>
#include <Actor/IActorEventListener.h>
#include <CryAISystem/IAgent.h>
//#include <Actor/Character/Movement/CharacterRotation.h>

class CPlayer;
class CActorMovementController;
class IActionController;
struct SAnimationContext;
struct SActorMovementRequest;
struct IEntityLocking;


/** Represents all of the available actor class types. */
enum EActorClassType
{
	///< Actor is a plain actor.
	EACT_ACTOR = 0,

	///< Actor is a player.
	EACT_PLAYER,

	///< Actor is a character.
	EACT_CHARACTER,

	///< Actor is a mount.
	EACT_MOUNT,

	///< Actor is a pet.
	EACT_PET,
};


/**
An implementation of the IActor interface. A CActor is an actor that represents a character within the game,
either an NPC or PC which can be controlled by a player.

Characters may have inventory.

\sa	CGameObjectExtensionHelper&lt;CActor, IActor&gt;
*/

// TODO: probably needs to also implement IInventoryListener to listen for inventory changes.

class CActor : public CGameObjectExtensionHelper<CActor, IActor, 40>, public IActorEventListener
{
public:
	/** This instance's default constructor. */
	CActor();

	/** This instance's default destructor. */
	virtual ~CActor();


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
	void PostUpdate(float frameTime) override {};
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
	void SetHealth(float health) override;


	/**
	Gets the current health of this instance.

	\return	The current health of this instance.
	*/
	float GetHealth() const override;


	/**
	Gets the current health as a rounded percentage.

	\return	The current health as a rounded percentage.
	*/
	int	GetHealthAsRoundedPercentage() const override;


	/**
	Sets the maximum amount of health this instance can have.

	\param	maxHealth	The maximum health you want this instance to have.
	*/
	void SetMaxHealth(float maxHealth) override;


	/**
	Gets the maximum amount of health this instance can have.

	\return	The maximum amount of health this instance can have.
	*/
	float GetMaxHealth() const override;


	/**
	Gets the current amount of armour this instance has.

	\return	The current amount of armour this instance has.
	*/
	int	GetArmor() const override;


	/**
	Gets the maximum amount of armour this instance can have.

	\return	The maximum amount of armour this instance can have.
	*/
	int	GetMaxArmor() const override;


	/**
	Gets team identifier.

	\return	The team identifier.
	*/
	int GetTeamId() const override { return m_teamId; };


	/**
	Gets whether this instance has fallen and needs revival.

	\return	Whether this instance has fallen and needs revival.
	*/
	bool IsFallen() const override;


	/**
	Gets whether this instance is dead.

	\return	Whether this instance is dead.
	*/
	bool IsDead() const override;


	/**
	Gets whether this instance is immune to damage.

	\return	Whether this instance is immune to damage.
	*/
	int	IsGod() override;


	/**
	Causes this instance to fall down based on the specified impact it has received.

	\param	hitPos	(Optional) the position of impact.
	*/
	void Fall(Vec3 hitPos = Vec3(0, 0, 0)) override;


	/**
	Gets whether this instance is allowed to perform a "landing bob".

	\return	Whether this instance is allowed to perform a "landing bob".
	*/
	bool AllowLandingBob() override;


	/**
	Play The specified action (animation). this method is DEPRECATED, and not used.

	\param	action   	The action (animation) to play.
	\param	extension	The file extension? of the animation file to load in order to play the specified action
	(animation).
	\param	looping  	(Optional) Specifies whether the specified action (animation) should loop of not.
	*/
	void PlayAction(const char *action, const char *extension, bool looping = false) override;


	/**
	Gets this instance's current animation graph state.

	An animation graph is used to decide animation logic (what animation should be played and on what condition they
	should be played).

	\return	This instance's current animation graph state.
	*/
	IAnimationGraphState* GetAnimationGraphState() override;


	/**
	Resets this instance's animation state.
	*/
	void ResetAnimationState() override;


	/**
	Calls the "ScriptEvent" script function on this instance's entity script. Used to forward responsibility of
	handling this event to this instance's entity script.

	\param	event	The script event that occurred.
	\param	value	The event specific floating-point value used by the specified event.
	\param	str  	(Optional) The event specific string value used by the specified event.
	*/
	void CreateScriptEvent(const char *event, float value, const char *str = NULL) override;


	/**
	Indicates that this instance should become aggressive towards the specified AI. Should maybe pull out this
	instance's weapon and aim it at the specified AI.

	\param	entityID	The EntityId of the AI to become aggressive towards.

	\return	True If Successful. False otherwise.
	*/
	bool BecomeAggressiveToAgent(EntityId entityID) override;


	/**
	Sets the facial alertness level of this instance. Should maybe call some sort of facial animation on this
	instance as well.

	\param	alertness	The new facial alertness level of this instance.
	*/
	void SetFacialAlertnessLevel(int alertness) override;


	/**
	Requests this instance to play the specified facial expression (animation).

	\param	pExpressionName		  	(Optional) The facial expression to play.
	\param [in,out]	sequenceLength	(Optional) When this method returns, holds the length (in seconds) of the started
	facial animation.
	*/
	void RequestFacialExpression(const char* pExpressionName = NULL, f32* sequenceLength = NULL) override;


	/**
	Pre-caches The specified facial expression.

	\param	pExpressionName	The name of the facial expression to pre-cache.
	*/
	void PrecacheFacialExpression(const char* pExpressionName) override;


	/**
	Gets the AI that is currently held by this instance.

	\return	The AI that is currently held by this instance.
	*/
	EntityId GetGrabbedEntityId() const override;


	/**
	Hides or shows all of this instance's attachments (typically weapons, but not necessarily).

	\param	isHiding	Specifies whether the attachments should be hidden or not. True to hide, false to show.
	*/
	void HideAllAttachments(bool isHiding) override;


	/**
	Sets the position of the specified limb using IK.

	\param	pLimbName	The name of the limb to set the position of.
	\param	goalPos  	The position to set the specified limb.
	\param	priority 	The priority to used while setting the specified limb's position.
	*/
	void SetIKPos(const char *pLimbName, const Vec3& goalPos, int priority) override;


	/**
	Sets the rotation of this instance's vehicle view.

	\param	viewRotation	The rotation to set the vehicle view to.
	*/
	void SetViewInVehicle(Quat viewRotation) override;


	/**
	Sets The rotation of this instance's view.

	\param	rotation	The rotation to set the view to.
	*/
	void SetViewRotation(const Quat &rotation) override;


	/**
	Gets the rotation of this instance's view.

	\return	The rotation of this instance's view.
	*/
	Quat GetViewRotation() const override;


	/**
	Gets whether the specified AI is a friend of this instance or not.

	\param	entityId				The EntityId of the AI you want to check for friendliness.
	\param	bUsingAIIgnoreCharacter	(Optional) Specifies whether the specified AI is ignoring this instance.

	\return	True If the specified AI is a friend of this instance. False otherwise.
	*/
	bool IsFriendlyEntity(EntityId entityId, bool bUsingAIIgnoreCharacter = true) const override;


	/**
	Gets this instance's local-space eye position (for a human, this is typically Vec3 (0, 0, 1.76f)).
	
	The code will first attempt to return a "#camera" helper if there is one. If only one eye is available (left_eye,
	right_eye) then that is used as the local position. In the case of two eyes, the position is the average of the two
	eyes.
	
	Position is calculated each time using the attachment manager, so it will be better to cache the results if you need
	to call this a few times in an update.
	
	\return This instance's local-space eye position.
	**/
	Vec3 GetLocalEyePos() const override;


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
	void CameraShake(float angle, float shift, float duration, float frequency, Vec3 pos, int ID, const char* source = "") override;


	/**
	Gets the currently holstered item of this instance.

	\return The currently holstered item of this instance.
	**/
	IItem* GetHolsteredItem() const override;


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
	void HolsterItem(bool holster, bool playSelect = true, float selectSpeedBias = 1.0f, bool hideLeftHandObject = true) override;


	/**
	Gets current item.

	\param	includeVehicle true to include, false to exclude the vehicles in the search.

	\return null if it fails, else the current item.

	\sa		GetCurrentItemId for obtaining the Id, since this is a more direct call into the item system.
	**/
	IItem* GetCurrentItem(bool includeVehicle = false) const override;


	/**
	Drop the specified item to the ground.

	\param	itemId		 The EntityId of the item to drop.
	\param	impulseScale The amount of impulse to apply to the dropped item.
	\param	selectNext   Specifies whether to select the next item.
	\param	byDeath		 Specifies whether this drop was the result of this instance being killed.

	\return True if the specified item was dropped. False otherwise.
	**/
	bool DropItem(EntityId itemId, float impulseScale = 1.0f, bool selectNext = true, bool byDeath = false) override;


	/**
	Gets this instance's inventory.

	\return This instance's inventory.
	**/
	IInventory* GetInventory() const override;


	/**
	Notifies this instance's entity script? That this instance's current item has changed.

	\param [in,out]	newItem The newly equipped item.
	**/
	void NotifyCurrentItemChanged(IItem* newItem) override;


	/**
	Gets this instance's movement controller (the class instance that controls how this instance moves around).

	\return	This instance's movement controller.
	*/
	IMovementController* GetMovementController() const override;


	/**
	Links this instance to the specified vehicle.

	\param	vehicleId	The EntityId of the vehicle to link this instance to.

	\return	The IEntity of the newly linked vehicle.
	*/
	IEntity* LinkToVehicle(EntityId vehicleId) override;


	/**
	Gets this instance's currently linked entity.

	\return	This instance's currently linked entity.
	*/
	IEntity* GetLinkedEntity() const override;


	/**
	Gets this instance's current spectator mode.

	\return	This instance's current spectator mode.
	*/
	uint8 GetSpectatorMode() const override;


	/**
	Gets whether this instance is using third person or not (typically means this instance is using the third person
	model, not the first person arms).

	\return	Whether This Instance Is Using 3RD Person Or Not.
	*/
	bool IsThirdPerson() const override;


	/**
	Toggles third person mode of this instance (typically means to toggle between third person and first person models).
	*/
	void ToggleThirdPerson() override;


	/**
	Query if this object is attached to a player. This does not have to be the local player, just as long as a player
	is attached.

	WARNING! Older code might try and cast this object to a player after receiving a true result here. Need to weed
	that code out and replace with another method.

	\return	true if player, false if not.
	*/
	bool IsPlayer() const override;


	/**
	Gets whether this instance is the "client actor" or not.

	\return	Whether this instance is the "client actor" or not.
	*/
	bool IsClient() const override;


	/**
	Gets whether this instance is currently migrating servers or not.

	\return	Whether this instance is currently migrating servers or not.
	*/
	bool IsMigrating() const override;


	/**
	Sets whether this instance is migrating servers or not.

	\param	isMigrating	specifies whether this instance is migrating servers or not. True if migrating, false otherwise.
	*/
	void SetMigrating(bool isMigrating) override;


	/**
	Initializes this instance. Used to initialize the local player (if this instance is the local player).
	*/
	void InitLocalPlayer() override;


	/**
	Gets the name of this instance's actor class (e.g. "CCharacter").

	NOTE: This must be overridden in derived classes.

	\return The name of this instance's actor class.
	**/
	const char* GetActorClassName() const override { return "CActor"; };


	/**
	Gets this instance's actor class type.

	NOTE: This must be overridden in derived classes.

	/sa EActorClassType.

	\return	This instance's actor class type.
	*/
	ActorClass GetActorClass() const override { return EActorClassType::EACT_ACTOR; };


	/**
	Gets the name of this instance's entity class.

	\return	The name of this instance's entity class.
	*/
	const char* GetEntityClassName() const override;


	/**
	Serializes this instance to/from the specified XML.

	\param [in,out]	node	The XML to serialize to/from.
	\param	bLoading		Specifies whether we are serializing to or serializing from the specified XML.
	*/
	void SerializeXML(XmlNodeRef& node, bool bLoading) override;


	/**
	Serializes this instance between levels.

	\param [in,out]	ser	The serializer to serialize to/from.
	*/
	void SerializeLevelToLevel(TSerialize &ser) override;


	/**
	Gets the animated character of this instance.

	\return	The animated character of this instance.
	*/
	IAnimatedCharacter* GetAnimatedCharacter() override;


	/**
	Gets the non-modifiable version of the animated character of this instance.

	\return	The non-modifiable version of the animated character of this instance.
	*/
	const IAnimatedCharacter* GetAnimatedCharacter() const override;


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
	void PlayExactPositioningAnimation(const char* sAnimationName, bool bSignal, const Vec3& vPosition,
		const Vec3& vDirection, float startWidth, float startArcAngle, float directionTolerance) override;


	/**
	Cancels the currently playing exact positioning animation.
	*/
	void CancelExactPositioningAnimation() override;


	/**
	Play the specified animation.

	\param	sAnimationName	The name of the animation you want to play.
	\param	bSignal		  	Specifies whether to notify other AI's about the specified animation?.
	*/
	void PlayAnimation(const char* sAnimationName, bool bSignal) override;


	/**
	Enables "Time Demo" mode (DEPRECATED?).

	\param	bTimeDemo	Specifies whether to enable "Time Demo" mode or not.
	*/
	void EnableTimeDemo(bool bTimeDemo) override;


	/**
	Switch this instance to be a spectator. Only valid if "Time Demo" mode is also enabled.

	\param	activate	Specifies whether to activate spectator mode or not.
	*/
	void SwitchDemoModeSpectator(bool activate) override;


	/**
	Gets this instance's currently linked vehicle.

	\return	This instance's currently linked vehicle.
	*/
	IVehicle* GetLinkedVehicle() const override;


	/**
	Automatically called by the IAISystem? When this instance's AI Proxy has been enabled or disabled.

	\param	enabled	Specifies whether this instance's AI Proxy was enabled or disabled.
	*/
	void OnAIProxyEnabled(bool enabled) override;


	/**
	Automatically called by the IEntitySystem? When this instance's entity has been returned to the entity pool.
	*/
	void OnReturnedToPool() override;


	/**
	Automatically called by the IEntitySystem? When this instance's entity is about to be spawned from the
	entity pool. Used To Prepare This Instance's Entity Before It Is Spawned From The entity pool.
	*/
	void OnPreparedFromPool() override;


	/**
	Gets whether this instance should not play weapon sounds if it has been stimulated.

	\return	Whether this instance should not play weapon sounds if it has been stimulated.
	*/
	bool ShouldMuteWeaponSoundStimulus() const override;


	/**
	Automatically called by the IEntitySystem when this instance's entity has been reused.

	\param [in,out]	pEntity	The entity that has been reused.
	\param	params		   	The parameters that were used to spawn the specified entity.
	*/
	void OnReused(IEntity *pEntity, SEntitySpawnParams &params) override;


	// ***
	// *** IActorEventListener
	// ***

public:

	/**
	Executes the special move action. Special moves are defined in an enum.

	\param [in,out]	pActor If non-null, the actor.
	\param	move		   The move.
	**/

	// TODO: Since there are just two special moves, consider making them full events instead.
	void OnSpecialMove(IActor* pActor, IActorEventListener::ESpecialMove move) override;


	/**
	The actor has died.

	\param [in,out]	pActor If non-null, the actor.
	\param	bIsGod		   true if this instance is god.
	**/
	void OnDeath(IActor* pActor, bool bIsGod) override;


	/**
	The actor has been revived.

	\param [in,out]	pActor If non-null, the actor.
	\param	bIsGod		   true if this instance is god.
	**/
	void OnRevive(IActor* pActor, bool bIsGod) override;


	/**
	The actor has entered a vehicle.

	\param [in,out]	pActor	    If non-null, the actor.
	\param	strVehicleClassName Name of the vehicle class.
	\param	strSeatName		    Name of the seat.
	\param	bThirdPerson	    true if we are in third person.
	**/
	void OnEnterVehicle(IActor* pActor, const char* strVehicleClassName, const char* strSeatName, bool bThirdPerson) override;


	/**
	The actor has exited a vehicle.

	\param [in,out]	pActor If non-null, the actor.
	**/
	void OnExitVehicle(IActor* pActor) override;


	/**
	The actor's health has changed.

	\param [in,out]	pActor If non-null, the actor.
	\param	newHealth	   The new health.
	**/
	void OnHealthChanged(IActor* pActor, float newHealth) override;


	/**
	The actor has picked up an item.

	\param [in,out]	pActor If non-null, the actor.
	\param	itemId		   Identifier for the item.
	**/
	void OnItemPickedUp(IActor* pActor, EntityId itemId) override;


	/**
	The actor has used an item.

	\param [in,out]	pActor If non-null, the actor.
	\param	itemId		   Identifier for the item.
	**/
	void OnItemUsed(IActor* pActor, EntityId itemId) override;


	/**
	The actor has dropped an item.

	\param [in,out]	pActor If non-null, the actor.
	\param	itemId		   Identifier for the item.
	**/
	void OnItemDropped(IActor* pActor, EntityId itemId) override;


	/**
	The actor has changed stance.

	\param [in,out]	pActor If non-null, the actor.
	\param	stance		   The stance.
	**/
	void OnStanceChanged(IActor* pActor, EStance stance) override;


	/**
	The actor has toggled between third person and first person view modes.

	\param [in,out]	pActor If non-null, the actor.
	\param	bThirdPerson   true to third person.
	**/
	void OnToggleThirdPerson(IActor* pActor, bool bThirdPerson) override;


	/**
	The actor's sprint stamina has changed.

	\param [in,out]	pActor If non-null, the actor.
	\param	newStamina	   The new stamina.
	**/
	void OnSprintStaminaChanged(IActor* pActor, float newStamina) override;


	// ***
	// *** CActor
	// ***

public:

	/**
	Gets actor statistics.

	\return	null if it fails, else the actor statistics.
	*/
	SActorState* GetActorState() { return &m_actorState; };


	/**
	Gets actor statistics.

	\return	null if it fails, else the actor statistics.
	*/
	const SActorState* GetActorState() const { return &m_actorState; };


	/**
	Gets actor physics.

	\return	null if it fails, else the actor physics.
	*/
	SActorPhysics* GetActorPhysics() { return &m_actorPhysics; };


	/**
	Gets actor physics.

	\return	null if it fails, else the actor physics.
	*/
	const SActorPhysics* GetActorPhysics() const { return &m_actorPhysics; };


	/**
	Provides access to our movement request object.

	This is used extensively by the movement state machine to store state as it's calculated / updated. Some states will
	make changes to the move request object e.g. flying and ladders. Most changes are enacted by the ground movement
	state, as you would generally expect. It's final state will be fed into the FinalizeMovementRequest function to pass
	the requested movement into the animation system.

	\return The move request.
	**/
	ILINE SCharacterMoveRequest& GetMoveRequest() { return m_moveRequest; }


	/**
	Gets the actors's pre-determined fate.

	\return	The fate.
	*/
	ILINE const CFate& GetFate() { return m_fate; }


protected:

	/**
	Gets current item identifier.

	\param	includeVehicle true to include, false to exclude the vehicles in the search.

	\return The current item identifier.
	**/
	EntityId GetCurrentItemId(bool includeVehicle = false) const;


	/** Called to indicate the entity must reset itself. This is often done during PostInit() and
	additionally by the editor when you both enter and leave game mode. */
	virtual void OnReset();

	/**
	Physicalies this instance.

	\return	True if physicalization was successful. False otherwise.
	*/
	virtual bool Physicalize();


	/**
	Pre physics update.
	*/
	virtual void PrePhysicsUpdate();


	/**
	Updates to the animation state described by frameMovementParams.

	\param	frameMovementParams	A variable-length parameters list containing frame movement parameters.
	*/
	virtual void UpdateAnimationState(const SActorMovementRequest& movementRequest);


	/** The movement controller. */
	CActorMovementController* m_pMovementController { nullptr };

	/** The actor physics. */
	SActorPhysics m_actorPhysics;


private:

	/** Specifies whether this instance is the client actor. */
	bool m_isClient { false };

	/** True if this actor is using a third person camera. */
	bool m_bIsThirdPerson { false };

	/** The actor's inventory. */
	IInventory* m_pInventory { nullptr };

	/** The actor's animated character. */
	IAnimatedCharacter* m_pAnimatedCharacter { nullptr };

	/** The action controller. */
	IActionController* m_pActionController { nullptr };

	/** Context for the animation. */
	SAnimationContext* m_pAnimationContext { nullptr };

	// HACK: to test switching movement and idle fragments. Should query physics instead.
	// Keeping the actions here allows me to stop them, which is good for testing, but wrong in
	// so many ways.
	bool m_wasMovingLastFrame { false };
	IActionPtr m_pActionIdle;
	IActionPtr m_pActionMove;

	/** Identifier for the team. */
	int m_teamId { 0 };

	/** If a player is controlling this character, this pointer will be valid. */
	CPlayer* m_pAttachedPlayer { nullptr };

	/** The current state for a character. This is shared by a lot of the state machine code. */
	SActorState m_actorState;

	/** The move request keeps track of how we wish to move this character based on input, state machine, and movement controllers. */
	SCharacterMoveRequest m_moveRequest {};

	/** A class that assists in working out animated character rotation. */
	//CCharacterRotation* m_characterRotation;

	/** The pre-determined fate for this actor. */
	CFate m_fate;


	// ***
	// *** AI / Player Control
	// ***

public:
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

private:
	/** true if this object is controlled by an AI. */
	bool m_isAIControlled;


	// ***
	// *** Stances
	// ***

public:

	ILINE EStance GetStance() const
	{
		return m_stance;
	}


	ILINE const SActorStance* GetStanceInfo(EStance stance) const
	{
		if (stance < 0 || stance > STANCE_LAST)
			return &m_defaultStance;

		return &m_stances [stance];
	}

	EStance m_stance;
	EStance m_desiredStance;
	static SActorStance m_defaultStance;
	SActorStance m_stances [STANCE_LAST];


	// ***
	// *** Life-cycle
	// ***

public:
	enum EReasonForRevive
	{
		RFR_FromInit,
		RFR_StartSpectating,
		RFR_Spawn,
		RFR_ScriptBind,
	};


	/** Resets the character to an initial state. */
	virtual void Reset();


	/** Kill the character. */
	virtual void Kill();


	/**
	Revive the character from death.

	\param	reasonForRevive	the reason for revive.
	*/
	virtual void Revive(EReasonForRevive reasonForRevive = RFR_Spawn);


	// ***
	// *** Hierarchical State Machine Support - this allows us to abstract the HSM away from the base
	// *** actor class - giving the flexibility to use different state machines for different
	// *** derived classes e.g. mounts
	// ***

public:

	/**
	Select movement hierarchy for our HSM.
	*/
	virtual void SelectMovementHierarchy() = 0;

	/** Release the HSM. */
	// TODO: Why does this need an empty implementation? If fails to build if I don't provide one.
	virtual void MovementHSMRelease() {};

	/** Init the HSM. */
	virtual void MovementHSMInit() = 0;

	/** Serialize the HSM. */
	virtual void MovementHSMSerialize(TSerialize ser) = 0;

	/** Update the HSM. */
	virtual void MovementHSMUpdate(SEntityUpdateContext& ctx, int updateSlot) = 0;

	/** Reset the HSM. */
	virtual void MovementHSMReset() = 0;


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


	/**
	An action bar entry has been triggered. Take whatever action is appropriate.
	
	\param	playerId    Identifier for the player.
	\param	actionBarId Identifier for the action bar.
	**/
	void OnActionBarUse(EntityId playerId, int actionBarId);


private:
	/**
	Core part of the interactor lock routine. This is split out and private to ensure we can control extra parts
	required for both locking and unlocking.

	\param	lockId	Identifier for the lock.
	\param	lock  	true to lock, false to unlock.
	*/
	void LockInteractor(EntityId lockId, bool lock);


	/** An interactor which is used to handle 'Useable' / 'Used' logic in the world. */
	IEntityLocking* m_pInteractor { nullptr };
};
