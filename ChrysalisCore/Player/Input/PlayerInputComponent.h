#pragma once

#include "IPlayerInputComponent.h"
#include <IActionMapManager.h>
#include <Player/Player.h>


class CPlayer;
class CCameraManagerComponent;


class CPlayerInputComponent : public CGameObjectExtensionHelper <CPlayerInputComponent, IPlayerInputComponent>, public IActionListener
//class CPlayerInputComponent : public IPlayerInputComponent, public IActionListener
{
	CRY_ENTITY_COMPONENT_INTERFACE_AND_CLASS(CPlayerInputComponent, "PlayerInput", 0x2CFC1B6B78334AA8, 0x9A29C2C51B919DC0)

public:
	// IEntityComponent
	void Initialize() override;
	void ProcessEvent(SEntityEvent& event) override;
	uint64 GetEventMask() const { return BIT64(ENTITY_EVENT_UPDATE); }
	// ~IEntityComponent

	// #TODO: Refactor when 5.4 is released.
	virtual bool Init(IGameObject* pGameObject) override { SetGameObject(pGameObject); return true; }
	void PostInit(IGameObject * pGameObject) override;
	virtual void Update(SEntityUpdateContext& ctx, int updateSlot) override;
	void PostUpdate(float frameTime);


	// ***
	// ***  IPlayerInputComponent
	// ***

public:

	virtual void ResetMovementState();
	virtual void ResetActionState();

	Vec3 GetMovement(const Quat& baseRotation) override;
	Ang3 GetRotationDelta() override { return Ang3(m_lastPitchDelta, 0.0f, m_lastYawDelta); }
	float GetPitchDelta() override { return m_lastPitchDelta; }
	float GetYawDelta() override { return m_lastYawDelta; };
	
	// Separate values for delta from each device. Useful when some values need inversion based on circumstances.
	float GetMousePitchDelta() override { return m_lastMousePitchDelta; }
	float GetMouseYawDelta() override { return m_lastMouseYawDelta; };
	float GetXiPitchDelta() override { return m_lastXiPitchDelta; }
	float GetXiYawDelta() override { return m_lastXiYawDelta; };

	float GetZoomDelta() override { return m_lastZoomDelta; };
	
	/** Provides access to the raw input movement directions as a set of flags for forward, backward, left and right.
	See EMovementStateFlags for the relevant flags. */
	uint32 GetMovementStateFlags() const { return m_movementStateFlags; };

	Vec3 GetHeadMovement(const Quat& baseRotation) override { return Vec3(0.0f, 0.0f, 0.0f); }
	Ang3 GetHeadRotationDelta() override { return Ang3(0.0f, 0.0f, 0.0f); }
	float GetHeadPitchDelta() override { return 0.0f; }
	float GetHeadYawDelta() override { return 0.0f; }


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
	void OnAction(const ActionId& action, int activationMode, float value) override;


	/** After action. */
	virtual void AfterAction() {};


	// ***
	// ***  CPlayerInputComponent
	// ***

public:
	CPlayerInputComponent()	{};
	virtual ~CPlayerInputComponent();


protected:
	bool OnActionMoveLeft(EntityId entityId, const ActionId& actionId, int activationMode, float value);
	bool OnActionMoveRight(EntityId entityId, const ActionId& actionId, int activationMode, float value);
	bool OnActionMoveForward(EntityId entityId, const ActionId& actionId, int activationMode, float value);
	bool OnActionMoveBackward(EntityId entityId, const ActionId& actionId, int activationMode, float value);
	bool OnActionRotateYaw(EntityId entityId, const ActionId& actionId, int activationMode, float value);
	bool OnActionRotatePitch(EntityId entityId, const ActionId& actionId, int activationMode, float value);
	bool OnActionXIRotateYaw(EntityId entityId, const ActionId& actionId, int activationMode, float value);
	bool OnActionXIRotatePitch(EntityId entityId, const ActionId& actionId, int activationMode, float value);
	bool OnActionZoomIn(EntityId entityId, const ActionId& actionId, int activationMode, float value);
	bool OnActionZoomOut(EntityId entityId, const ActionId& actionId, int activationMode, float value);
	bool OnActionJump(EntityId entityId, const ActionId& actionId, int activationMode, float value);
	bool OnActionCrouch(EntityId entityId, const ActionId& actionId, int activationMode, float value);
	bool OnActionKneel(EntityId entityId, const ActionId& actionId, int activationMode, float value);
	bool OnActionSit(EntityId entityId, const ActionId& actionId, int activationMode, float value);
	bool OnActionSprint(EntityId entityId, const ActionId& actionId, int activationMode, float value);
	bool OnActionWalkJog(EntityId entityId, const ActionId& actionId, int activationMode, float value);
	bool OnActionItemUse(EntityId entityId, const ActionId& actionId, int activationMode, float value);
	bool OnActionItemPickup(EntityId entityId, const ActionId& actionId, int activationMode, float value);
	bool OnActionItemDrop(EntityId entityId, const ActionId& actionId, int activationMode, float value);
	bool OnActionInteraction(EntityId entityId, const ActionId& actionId, int activationMode, float value);


	/**
	Action handler for tossing an item. In this case toss means a light dismissive throw in the forward direction.

	\param	entityId	  	The entityId for the player who invoked this action.
	\param	actionId	  	Identifier for the action that was triggered by the player.
	\param	activationMode	The activation mode.
	\param	value		  	An optional value that may contain useful information for an action.

	\return	true if it succeeds, false if it fails.
	*/
	bool OnActionItemToss(EntityId entityId, const ActionId& actionId, int activationMode, float value);


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

	/**	Camera debug actions. **/
	bool OnActionCameraShiftUp(EntityId entityId, const ActionId& actionId, int activationMode, float value);
	bool OnActionCameraShiftDown(EntityId entityId, const ActionId& actionId, int activationMode, float value);
	bool OnActionCameraShiftLeft(EntityId entityId, const ActionId& actionId, int activationMode, float value);
	bool OnActionCameraShiftRight(EntityId entityId, const ActionId& actionId, int activationMode, float value);
	bool OnActionCameraShiftForward(EntityId entityId, const ActionId& actionId, int activationMode, float value);
	bool OnActionCameraShiftBackward(EntityId entityId, const ActionId& actionId, int activationMode, float value);

	bool OnActionInspectStart(EntityId entityId, const ActionId& actionId, int activationMode, float value);
	bool OnActionInspect(EntityId entityId, const ActionId& actionId, int activationMode, float value);
	bool OnActionInspectEnd(EntityId entityId, const ActionId& actionId, int activationMode, float value);

	/** The player. */
	CPlayer* m_pPlayer { nullptr };

	/** The camera that this instance uses. */
	CCameraManagerComponent* m_pCameraManager { nullptr };


private:

	/** Registers the action maps and starts to listen for action map events. */
	void RegisterActionMaps();

	void InitializeActionHandler();

	/** A static handler for the actions we are interested in hooking. */
	TActionHandler<CPlayerInputComponent> m_actionHandler;

	/** The movement mask. */
	uint32 m_movementStateFlags { EMovementStateFlags::None };

	/** The current stance they wish to be in. */
	uint32 m_stanceFlags { EStanceFlags::Standing };

	/** Drop incoming actions if this is false. Prevents actions arriving after we start to shut down from being serviced. */
	bool m_allowActions { false };

	/**
	Should we invert the Y axis for mouse camera movements? This is preferred by some players, particularly those
	using a flight yoke.
	*/
	int m_mouseInvertPitch { 0 };

	/** Mouse sensitivity for pitching and yawing. All pitch and yaw deltas will be factored by this value. */
	float m_mousePitchYawSensitivity { 1.0f };

	/** The delta (radians) of pitch requested via all inputs. This value is calculated when Update is called.*/
	float m_lastPitchDelta { 0.0f };

	/** The delta (radians) of yaw requested via all inputs.  This value is calculated when Update is called.*/
	float m_lastYawDelta { 0.0f };

	/** The delta (radians) of pitch requested using a mouse input.*/
	float m_mousePitchDelta { 0.0f };
	float m_lastMousePitchDelta { 0.0f };

	/** The delta (radians) of yaw requested using a mouse input. */
	float m_mouseYawDelta { 0.0f };
	float m_lastMouseYawDelta { 0.0f };

	/**
	This tracks the delta pitch (radians) from the XBox controller, if one is present.
	*/
	float m_xiPitchDelta { 0.0f };
	float m_lastXiPitchDelta { 0.0f };

	/**
	This tracks the delta yaw (radians) from the XBox controller, if one is present.
	*/
	float m_xiYawDelta { 0.0f };
	float m_lastXiYawDelta { 0.0f };

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
