#pragma once

#include "IPlayerInputComponent.h"
#include <IActionMapManager.h>
#include "Components/Player/PlayerComponent.h"
#include <DefaultComponents/Input/InputComponent.h>


namespace Chrysalis
{
class CPlayerComponent;
class CCameraManagerComponent;

class CPlayerInputComponent
	: public IPlayerInputComponent
{
protected:
	friend CChrysalisCorePlugin;
	static void Register(Schematyc::CEnvRegistrationScope& componentScope);

	// IEntityComponent
	void Initialize() override;
	void ProcessEvent(SEntityEvent& event) override;
	uint64 GetEventMask() const { return BIT64(ENTITY_EVENT_UPDATE); }
	// ~IEntityComponent

	void Update();

public:
	CPlayerInputComponent() {}
	virtual ~CPlayerInputComponent();

	static void ReflectType(Schematyc::CTypeDesc<CPlayerInputComponent>& desc);

	static CryGUID& IID()
	{
		static CryGUID id = "{B38ECC98-FEB2-467D-B78B-05743B1CA260}"_cry_guid;
		return id;
	}

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
	See EmovementDirectionFlags for the relevant flags. */
	TInputFlags GetMovementDirectionFlags() const { return m_inputFlags; };

	Vec3 GetHeadMovement(const Quat& baseRotation) override { return Vec3(0.0f, 0.0f, 0.0f); }
	Ang3 GetHeadRotationDelta() override { return Ang3(0.0f, 0.0f, 0.0f); }
	float GetHeadPitchDelta() override { return 0.0f; }
	float GetHeadYawDelta() override { return 0.0f; }


	/**
	Returns an instance of the special keystroke listener so other code sections can subscribe to special keystrokes.

	\return The special listener.
	**/
	TListener<IInputSpecialListener> GetSpecialListener() override { return m_listenersSpecial; }


	// ***
	// ***  CPlayerInputComponent
	// ***

protected:
	void OnActionEscape(int activationMode, float value);
	void OnActionExamine(int activationMode, float value);

	void OnActionRotateYaw(int activationMode, float value);
	void OnActionRotatePitch(int activationMode, float value);
	void OnActionXIRotateYaw(int activationMode, float value);
	void OnActionXIRotatePitch(int activationMode, float value);

	void OnActionJump(int activationMode, float value);
	void OnActionCrouchToggle(int activationMode, float value);
	void OnActionCrawlToggle(int activationMode, float value);
	void OnActionKneelToggle(int activationMode, float value);
	void OnActionSitToggle(int activationMode, float value);
	void OnActionSprintToggle(int activationMode, float value);
	void OnActionWalkJog(int activationMode, float value);

	void OnActionItemUse(int activationMode, float value);
	void OnActionItemPickup(int activationMode, float value);
	void OnActionItemDrop(int activationMode, float value);
	void OnActionInteraction(int activationMode, float value);


	/**
	Action handler for tossing an item. In this case toss means a light dismissive throw in the forward direction.

	\param	entityId	  	The entityId for the player who invoked this action.
	\param	actionId	  	Identifier for the action that was triggered by the player.
	\param	activationMode	The activation mode.
	\param	value		  	An optional value that may contain useful information for an action.

	\return	true if it succeeds, false if it fails.
	*/
	void OnActionItemToss(int activationMode, float value);


	/**
	Generic action bar action.

	\param	entityId	   Identifier for the entity.
	\param	actionId	   Identifier for the action.
	\param	activationMode The activation mode.
	\param	value		   Provides the Id number of the action bar clicked

	\return true if it succeeds, false if it fails.
	**/
	void OnActionBar(int activationMode, int buttonId);


	/**
	Generic numberpad action.

	\param	entityId	   Identifier for the entity.
	\param	actionId	   Identifier for the action.
	\param	activationMode The activation mode.
	\param	value		   Provides the Id number of the action bar clicked

	\return true if it succeeds, false if it fails.
	**/
	void OnNumpad(int activationMode, int buttonId);

	/**	Camera debug actions. **/
	void OnActionCameraShiftUp(int activationMode, float value);
	void OnActionCameraShiftDown(int activationMode, float value);
	void OnActionCameraShiftLeft(int activationMode, float value);
	void OnActionCameraShiftRight(int activationMode, float value);
	void OnActionCameraShiftForward(int activationMode, float value);
	void OnActionCameraShiftBackward(int activationMode, float value);

	void OnActionInspectStart(int activationMode, float value);
	void OnActionInspect(int activationMode, float value);
	void OnActionInspectEnd(int activationMode, float value);

	/** The player. */
	CPlayerComponent* m_pPlayer { nullptr };

	/** The camera that this instance uses. */
	CCameraManagerComponent* m_pCameraManager { nullptr };


private:

	/** Registers the action maps and starts to listen for action map events. */
	void RegisterActionMaps();

	void HandleInputFlagChange(TInputFlags flags, int activationMode, EInputFlagType type = EInputFlagType::Hold);

	/** The input component */
	Cry::DefaultComponents::CInputComponent* m_pInputComponent = nullptr;

	/** The movement mask. */
	TInputFlags m_inputFlags { (TInputFlags) EInputFlag::None };

	/** Drop incoming actions if this is false. Prevents actions arriving after we start to shut down from being serviced. */
	// TODO: this was to fix a bug in cryaction, and can probably be removed now.
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

	/** Listeners for special keystrokes. */
	TListener<IInputSpecialListener> m_listenersSpecial;
};
}