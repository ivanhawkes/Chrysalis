#pragma once

#include "PlayerInputComponent.h"
#include <IActionMapManager.h>
#include <DefaultComponents/Input/InputComponent.h>
#include "Components/Player/PlayerComponent.h"
//#include "Utility/Listener.h"


namespace Chrysalis
{
class CPlayerComponent;
class CCameraManagerComponent;

enum class EInputFlagType
{
	Hold = 0,
	Toggle
};

typedef uint8 TInputFlags;

enum class EInputFlag
	: TInputFlags
{
	None = 0,
	Left = 1 << 0,
	Right = 1 << 1,
	Forward = 1 << 2,
	Backward = 1 << 3
};


class CPlayerInputComponent
	: public IEntityComponent
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
	CPlayerInputComponent() = default;
	virtual ~CPlayerInputComponent() = default;

	static void ReflectType(Schematyc::CTypeDesc<CPlayerInputComponent>& desc);

	static CryGUID& IID()
	{
		static CryGUID id = "{B38ECC98-FEB2-467D-B78B-05743B1CA260}"_cry_guid;
		return id;
	}


	/**
	Given the current input state, calculate a vector that represents the direction the player wishes their character
	to move. The vector is normalised. Movement will only be affected along the X and Y axis, since we are not presently
	processing Z input (up). This should be sufficient for most RPG style games.

	\param	baseRotation	The vector will be calculated using this rotation as a base and then applying the input
							requests relative to this direction.

	\return The calculated movement direction.
	**/
	Vec3 GetMovement(const Quat& baseRotation);


	/**
	Given the current input state, calculate an angular vector that represents the rotation the player has requested
	using the mouse / xbox controller / etc.

	This is typically used to rotate the character and the camera.

	\return The rotation.
	**/
	Ang3 GetRotationDelta() { return Ang3(m_lastPitchDelta, 0.0f, m_lastYawDelta); }


	/**
	Gets the combined pitch delta from all devices.

	\return The pitch delta.
	**/
	float GetPitchDelta() { return m_lastPitchDelta; }


	/**
	Gets the combined yaw delta from all devices.

	\return The yaw delta.
	**/
	float GetYawDelta() { return m_lastYawDelta; };


	/**
	Gets the pitch delta from the mouse.

	\return The pitch delta.
	**/
	float GetMousePitchDelta() { return m_lastMousePitchDelta; }


	/**
	Gets the yaw delta from the mouse.

	\return The yaw delta.
	**/
	float GetMouseYawDelta() { return m_lastMouseYawDelta; };


	/**
	Gets the pitch delta from the XBOX controller.

	\return The pitch delta.
	**/
	float GetXiPitchDelta() { return m_lastXiPitchDelta; }


	/**
	Gets the yaw delta from the XBOX controller.

	\return The yaw delta.
	**/
	float GetXiYawDelta() { return m_lastXiYawDelta; };


	/**
	Provides access to the raw input movement directions as a set of flags for forward, backward, left and right. See
	EmovementDirectionFlags for the relevant flags.

	\return The movement direction flags.
	**/
	TInputFlags GetMovementDirectionFlags() const { return m_inputFlags; };


	///** Listen for 'special' keys and be notified when they are input e.g. ESC, Examine. */
	//struct IInputSpecialListener
	//{
	//	virtual ~IInputSpecialListener() = default;

	//	virtual void OnInputSpecialEsc() = 0;
	//	virtual void OnInputSpecialExamine() = 0;
	//};


	///** Get the manager for the listener.*/
	//TListener<IInputSpecialListener*>GetListenerManager() { return m_listenersSpecial; }

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
	Cry::DefaultComponents::CInputComponent* m_pInputComponent { nullptr };

	/** The movement mask. */
	TInputFlags m_inputFlags { (TInputFlags)EInputFlag::None };

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

	/** Listens for special keystrokes. */
	//TListener<IInputSpecialListener*> m_listenersSpecial;
};
}