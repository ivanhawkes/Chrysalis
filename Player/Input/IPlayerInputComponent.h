/**
\file d:\CRYENGINE\Code\ChrysalisSDK\Actor\Player\Input\IPlayerInputComponent.h

Declares the IPlayerInputComponent interface.
**/
#pragma once

#include <Entities/Helpers/ISimpleExtension.h>


enum EMovementStateFlags
{
	None =		0,
	Forward =	BIT(0),
	Backward =	BIT(1),
	Left =		BIT(2),
	Right =		BIT(3),
};


enum EStanceFlags
{
	Standing,
	Crouching,
	Sitting,
	Kneeling
};


struct IPlayerInputComponent : public ISimpleExtension
{
	enum class EInputType
	{
		Null,
		Player,
		NetPlayer,
		Ai,
		DedicatedServer,
		_LAST,
	};


	virtual ~IPlayerInputComponent() {};


	/**
	Reset the movements to zero state.
	*/
	virtual void ResetMovementState() = 0;


	/**
	Reset the actions resulting from movement state to zero state.
	*/
	virtual void ResetActionState() = 0;


	/**
	Given the current input state, calculate a vector that represents the direction the player wishes their
	character to move. The vector is normalised. Movement will only be affected along the X and Y axis,
	since we are not presently processing Z input (up). This should be sufficient for most RPG style games.

	\param	baseRotation	The vector will be calculated using this rotation as a base and then applying the input
	requests relative to this direction.

	\return The calculated movement direction.
	**/
	virtual Vec3 GetMovement(const Quat& baseRotation) = 0;


	/**
	Given the current input state, calculate an angular vector that represents the rotation the player has requested
	using the mouse / xbox controller / etc.
	
	This is typically used to rotate the character and the camera.
	
	\return The rotation.
	**/
	virtual Ang3 GetRotationDelta() = 0;


	/**
	Gets pitch delta.
	
	\return The pitch delta.
	**/
	virtual float GetPitchDelta() = 0;


	/**
	Gets yaw delta.
	
	\return The yaw delta.
	**/
	virtual float GetYawDelta() = 0;


	/**
	Gets number of times the player has requested a change in zoom level since the last frame. Cameras can query this
	value and use it to adjust their zoom.
	
	\return The zoom delta.
	**/
	virtual float GetZoomDelta () = 0;


	/**
	Gets movement state flags.

	\return The movement state flags.
	**/
	virtual uint32 GetMovementStateFlags() = 0;


	// ***
	// *** HMD based head tracking. Provide the ability to handle head movement separate to body movement.
	// ***

	virtual Vec3 GetHeadMovement(const Quat& baseRotation) = 0;

	virtual Ang3 GetHeadRotationDelta() = 0;

	virtual float GetHeadPitchDelta() = 0;

	virtual float GetHeadYawDelta() = 0;
};
