#pragma once

#include "IPlayerInputComponent.h"
#include <IGameObject.h>


struct IActionListener;
class CPlayer;


class CPlayerInputComponent : public CGameObjectExtensionHelper <CPlayerInputComponent, IPlayerInputComponent>, public IActionListener
{
public:

	// ***
	// *** ISimpleExtension
	// ***

	void PostInit(IGameObject * pGameObject) override;
	void Update(SEntityUpdateContext& ctx, int updateSlot) override;
	void PostUpdate(float frameTime) override;
	void HandleEvent(const SGameObjectEvent &event) override;


	// ***
	// ***  IPlayerInputComponent
	// ***

public:

	/** Reset the movements to zero state. */
	virtual void ResetMovementState();


	/** Reset the actions resulting from movement state to zero state. */
	virtual void ResetActionState();


	/**
	Given the current input state, calculate a vector that represents the direction the player wishes their
	character to move. The vector is normalised. Movement will only be affected along the X and Y axis,
	since we are not presently processing Z input (up). This should be sufficient for most RPG style games.

	\param	baseRotation	The vector will be calculated using this rotation as a base and then applying the input
	requests relative to this direction.

	\return The calculated movement direction.
	**/
	Vec3 GetMovement(const Quat& baseRotation) override;


	/**
	Given the current input state, calculate an angular vector that represents the rotation the player has
	requested using the mouse / xbox controller / etc.

	This is typically used to rotate the character and the camera.

	\return The rotation.
	**/
	Ang3 GetRotationDelta() override;


	/**
	Gets pitch delta.

	\return The pitch delta.
	**/
	float GetPitchDelta() override;


	/**
	Gets yaw delta.

	\return The yaw delta.
	**/
	float GetYawDelta() override;


	/**
	Gets number of times the player has requested a change in zoom level since the last frame. Cameras can query this
	value and use it to adjust their zoom. Value is set in stone on PrePhysicsUpdate () so it can be consistently
	queried during the Update() routine. It's provided as a float, even though currently implementation is integral
	steps. This will give us more fine control if needed later.


	\return The zoom delta.
	**/
	float GetZoomDelta() override {	return m_lastZoomDelta; };


	/**
	Gets movement state flags.
	
	\return The movement state flags.
	**/
	uint32 GetMovementStateFlags () { return m_movementStateFlags; };

	
	Vec3 GetHeadMovement(const Quat& baseRotation) override;

	Ang3 GetHeadRotationDelta() override;

	float GetHeadPitchDelta() override;

	float GetHeadYawDelta() override;

	
	// ***
	// *** IActionListener
	// ***

public:

	/**
	Handles the action event.

	\param	action		  	The action.
	\param	activationMode	The activation mode.
	\param	value		  	An optional value that may contain useful information for an action.
	*/
	virtual void OnAction(const ActionId& action, int activationMode, float value);


	/** After action. */
	virtual void AfterAction() {};


	// ***
	// ***  CPlayerInputComponent
	// ***

public:

	CPlayerInputComponent() {};
	virtual ~CPlayerInputComponent();


protected:
	/**
	Action handler for moving left.

	\param	entityId	  	The entityId for the player who invoked this action.
	\param	actionId	  	Identifier which describes the type of action that is occuring.
	\param	activationMode	The activation mode.
	\param	value		  	An optional value that may contain useful information for an action.

	\return	true if it succeeds, false if it fails.
	*/
	bool OnActionMoveLeft(EntityId entityId, const ActionId& actionId, int activationMode, float value);


	/**
	Action handler for moving right.

	\param	entityId	  	The entityId for the player who invoked this action.
	\param	actionId	  	Identifier for the action that was triggered by the player.
	\param	activationMode	The activation mode.
	\param	value		  	An optional value that may contain useful information for an action.

	\return	true if it succeeds, false if it fails.
	*/
	bool OnActionMoveRight(EntityId entityId, const ActionId& actionId, int activationMode, float value);


	/**
	Action handler for moving forward.

	\param	entityId	  	The entityId for the player who invoked this action.
	\param	actionId	  	Identifier for the action that was triggered by the player.
	\param	activationMode	The activation mode.
	\param	value		  	An optional value that may contain useful information for an action.

	\return	true if it succeeds, false if it fails.
	*/
	bool OnActionMoveForward(EntityId entityId, const ActionId& actionId, int activationMode, float value);


	/**
	Action handler for moving backward.

	\param	entityId	  	The entityId for the player who invoked this action.
	\param	actionId	  	Identifier for the action that was triggered by the player.
	\param	activationMode	The activation mode.
	\param	value		  	An optional value that may contain useful information for an action.

	\return	true if it succeeds, false if it fails.
	*/
	bool OnActionMoveBackward(EntityId entityId, const ActionId& actionId, int activationMode, float value);


	/**
	Action handler for mouse yawing.

	\param	entityId	  	Identifier for the entity.
	\param	actionId	  	Identifier for the action.
	\param	activationMode	The activation mode.
	\param	value		  	The value.

	\return	true if it succeeds, false if it fails.
	*/
	bool OnActionRotateYaw(EntityId entityId, const ActionId& actionId, int activationMode, float value);


	/**
	Action handler for mouse pitching.

	\param	entityId	  	Identifier for the entity.
	\param	actionId	  	Identifier for the action.
	\param	activationMode	The activation mode.
	\param	value		  	The value.

	\return	true if it succeeds, false if it fails.
	*/
	bool OnActionRotatePitch(EntityId entityId, const ActionId& actionId, int activationMode, float value);


	/**
	Action handler for XBox controller yawing.

	\param	entityId	  	Identifier for the entity.
	\param	actionId	  	Identifier for the action.
	\param	activationMode	The activation mode.
	\param	value		  	The value.

	\return	true if it succeeds, false if it fails.
	*/
	bool OnActionXIRotateYaw(EntityId entityId, const ActionId& actionId, int activationMode, float value);


	/**
	Action handler for XBox controller pitching.

	\param	entityId	  	Identifier for the entity.
	\param	actionId	  	Identifier for the action.
	\param	activationMode	The activation mode.
	\param	value		  	The value.

	\return	true if it succeeds, false if it fails.
	*/
	bool OnActionXIRotatePitch(EntityId entityId, const ActionId& actionId, int activationMode, float value);


	/**
	Action handler for mouse zooming in.

	\param	entityId	  	Identifier for the entity.
	\param	actionId	  	Identifier for the action.
	\param	activationMode	The activation mode.
	\param	value		  	The value.

	\return	true if it succeeds, false if it fails.
	*/
	bool OnActionZoomIn(EntityId entityId, const ActionId& actionId, int activationMode, float value);


	/**
	Action handler for mouse zooming out.

	\param	entityId	  	Identifier for the entity.
	\param	actionId	  	Identifier for the action.
	\param	activationMode	The activation mode.
	\param	value		  	The value.

	\return	true if it succeeds, false if it fails.
	*/
	bool OnActionZoomOut(EntityId entityId, const ActionId& actionId, int activationMode, float value);


	/**
	Action handler for jumping.

	\param	entityId	  	The entityId for the player who invoked this action.
	\param	actionId	  	Identifier for the action that was triggered by the player.
	\param	activationMode	The activation mode.
	\param	value		  	An optional value that may contain useful information for an action.

	\return	true if it succeeds, false if it fails.
	*/
	bool OnActionJump(EntityId entityId, const ActionId& actionId, int activationMode, float value);


	/**
	Action handler for crouching.

	\param	entityId	  	The entityId for the player who invoked this action.
	\param	actionId	  	Identifier for the action that was triggered by the player.
	\param	activationMode	The activation mode.
	\param	value		  	An optional value that may contain useful information for an action.

	\return	true if it succeeds, false if it fails.
	*/
	bool OnActionCrouch(EntityId entityId, const ActionId& actionId, int activationMode, float value);


	/**
	Action handler for kneeling.

	\param	entityId	  	The entityId for the player who invoked this action.
	\param	actionId	  	Identifier for the action that was triggered by the player.
	\param	activationMode	The activation mode.
	\param	value		  	An optional value that may contain useful information for an action.

	\return	true if it succeeds, false if it fails.
	*/
	bool OnActionKneel(EntityId entityId, const ActionId& actionId, int activationMode, float value);


	/**
	Action handler for sitting.

	\param	entityId	  	The entityId for the player who invoked this action.
	\param	actionId	  	Identifier for the action that was triggered by the player.
	\param	activationMode	The activation mode.
	\param	value		  	An optional value that may contain useful information for an action.

	\return	true if it succeeds, false if it fails.
	*/
	bool OnActionSit(EntityId entityId, const ActionId& actionId, int activationMode, float value);


	/**
	Action handler for sprint.

	\param	entityId	  	The entityId for the player who invoked this action.
	\param	actionId	  	Identifier for the action that was triggered by the player.
	\param	activationMode	The activation mode.
	\param	value		  	An optional value that may contain useful information for an action.

	\return	true if it succeeds, false if it fails.
	*/
	bool OnActionSprint(EntityId entityId, const ActionId& actionId, int activationMode, float value);


	/**
	Action handler for toggling between walking and running / jogging.

	\param	entityId	  	The entityId for the player who invoked this action.
	\param	actionId	  	Identifier for the action that was triggered by the player.
	\param	activationMode	The activation mode.
	\param	value		  	An optional value that may contain useful information for an action.

	\return	true if it succeeds, false if it fails.
	*/
	bool OnActionWalkRun(EntityId entityId, const ActionId& actionId, int activationMode, float value);


	/**
	Action handler for the "use" verb for entities that provide a use function.
	
	\param	entityId	  	The entityId for the player who invoked this action.
	\param	actionId	  	Identifier for the action that was triggered by the player.
	\param	activationMode	The activation mode.
	\param	value		  	An optional value that may contain useful information for an action.
	
	\return	true if it succeeds, false if it fails.
	*/
	bool OnActionItemUse(EntityId entityId, const ActionId& actionId, int activationMode, float value);


	/**
	Action handler for picking up an item / entitiy.
	
	\param	entityId	  	The entityId for the player who invoked this action.
	\param	actionId	  	Identifier for the action that was triggered by the player.
	\param	activationMode	The activation mode.
	\param	value		  	An optional value that may contain useful information for an action.
	
	\return	true if it succeeds, false if it fails.
	*/
	bool OnActionItemPickup(EntityId entityId, const ActionId& actionId, int activationMode, float value);


	/**
	Action handler for dropping an item / entity.
	
	\param	entityId	  	The entityId for the player who invoked this action.
	\param	actionId	  	Identifier for the action that was triggered by the player.
	\param	activationMode	The activation mode.
	\param	value		  	An optional value that may contain useful information for an action.
	
	\return	true if it succeeds, false if it fails.
	*/
	bool OnActionItemDrop(EntityId entityId, const ActionId& actionId, int activationMode, float value);


	/**
	Action handler for throwing an item. General expectation is this will be used for "Pick and Throw" weapons but it
	might need to also handle regular weapons and even ordinary items.
	
	\param	entityId	  	The entityId for the player who invoked this action.
	\param	actionId	  	Identifier for the action that was triggered by the player.
	\param	activationMode	The activation mode.
	\param	value		  	An optional value that may contain useful information for an action.
	
	\return	true if it succeeds, false if it fails.
	*/
	bool OnActionItemThrow(EntityId entityId, const ActionId& actionId, int activationMode, float value);


	/**
	Generic action bar action.
	
	\param	entityId	   Identifier for the entity.
	\param	actionId	   Identifier for the action.
	\param	activationMode The activation mode.
	\param	value		   Provides the Id number of the action bar clicked
	
	\return true if it succeeds, false if it fails.
	**/
	bool OnActionBar(EntityId entityId, const ActionId& actionId, int activationMode, int buttonId);
	
	bool OnActionBar01(EntityId entityId, const ActionId& actionId, int activationMode, float value);
	bool OnActionBar02(EntityId entityId, const ActionId& actionId, int activationMode, float value);
	bool OnActionBar03(EntityId entityId, const ActionId& actionId, int activationMode, float value);
	bool OnActionBar04(EntityId entityId, const ActionId& actionId, int activationMode, float value);
	bool OnActionBar05(EntityId entityId, const ActionId& actionId, int activationMode, float value);
	bool OnActionBar06(EntityId entityId, const ActionId& actionId, int activationMode, float value);
	bool OnActionBar07(EntityId entityId, const ActionId& actionId, int activationMode, float value);
	bool OnActionBar08(EntityId entityId, const ActionId& actionId, int activationMode, float value);
	bool OnActionBar09(EntityId entityId, const ActionId& actionId, int activationMode, float value);
	bool OnActionBar10(EntityId entityId, const ActionId& actionId, int activationMode, float value);
	bool OnActionBar11(EntityId entityId, const ActionId& actionId, int activationMode, float value);
	bool OnActionBar12(EntityId entityId, const ActionId& actionId, int activationMode, float value);

private:

	/** Registers the action maps and starts to listen for action map events. */
	void RegisterActionMaps();

	void InitializeActionHandler();

	/** The player. */
	CPlayer* m_pPlayer { nullptr };

	/** A static handler for the actions we are interested in hooking. */
	TActionHandler<CPlayerInputComponent> m_actionHandler;

	/** The movement mask. */
	uint32 m_movementStateFlags { EMovementStateFlags::None };

	/** The current stance they wish to be in. */
	uint32 m_stanceFlags { EStanceFlags::Standing };

	/** true when the player wishes to sprint. */
	bool shouldSprint { false };

	/** true when the player wishes to run instead of walking. */
	bool shouldRun { false };

	/** The delta (radians) of pitch requested using a mouse input.*/
	float m_mousePitchDelta { 0.0f };

	/** The delta (radians) of yaw requested using a mouse input. */
	float m_mouseYawDelta { 0.0f };

	/** The delta (radians) of pitch requested via all inputs. This value is calculated when Update is called.*/
	float m_lastPitchDelta { 0.0f };

	/** The delta (radians) of yaw requested via all inputs.  This value is calculated when Update is called.*/
	float m_lastYawDelta { 0.0f };

	/**
	Should we invert the Y axis for mouse camera movements? This is preferred by some players, particularly those
	using a flight yoke.
	*/
	int m_mouseInvertPitch { 0 };

	/** Mouse sensitivity for pitching and yawing. All pitch and yaw deltas will be factored by this value. */
	float m_mousePitchYawSensitivity { 1.0f };

	/**
	This tracks the delta pitch (radians) from the XBox controller, if one is present.
	*/
	float m_xiPitchDelta { 0.0f };

	/**
	This tracks the delta yaw (radians) from the XBox controller, if one is present.
	*/
	float m_xiYawDelta { 0.0f };

	/**
	Filter pitch adjustments below this threshold (radians). This is useful for removing slight amounts of jitter on
	mouse movements and XBox controllers, making it easier to perform precise movements in only one axis.
	*/
	float m_pitchFilter { 0.0001f };
	
	/**
	Filter yaw adjustments below this threshold (radians). This is useful for removing slight amounts of jitter on
	mouse movements and XBox controllers, making it easier to perform precise movements in only one axis.
	*/
	float m_yawFilter { 0.0001f };

	/**
	Filter pitch adjustments below this threshold (radians). This is useful for removing slight amounts of jitter on
	mouse movements and XBox controllers, making it easier to perform precise movements in only one axis.
	*/
	float m_xiPitchFilter { 0.0001f };

	/**
	Filter yaw adjustments below this threshold (radians). This is useful for removing slight amounts of jitter on
	mouse movements and XBox controllers, making it easier to perform precise movements in only one axis.
	*/
	float m_xiYawFilter { 0.0001f };

	/**	Zoom delta. A value that indicates how much they wish to zoom in (negative values) or out (positive values). **/
	float m_zoomDelta { 0.0f };

	/**	Last zoom delta. **/
	float m_lastZoomDelta { 0.0f };
};
