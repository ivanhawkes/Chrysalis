/**
\file	Source\Actor\PlayerCamera\PlayerCamera.h

This is a camera management class. It creates several cameras, one of each type, and then provides features
for switching between the cameras. While the name implies the cameras are just for the player, in general they will
be able to follow any entity in the game.

The host entity will be used as a default for the entity which the camera operates from.
*/
#pragma once

#include <IGameObject.h>
#include "ICameraComponent.h"


namespace Chrysalis
{
/** A camera manager component. */
class CCameraManagerComponent
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

public:
	CCameraManagerComponent();
	virtual ~CCameraManagerComponent() {}

	static void ReflectType(Schematyc::CTypeDesc<CCameraManagerComponent>& desc);

	static CryGUID& IID()
	{
		static CryGUID id = "{D1204EE2-BF92-4817-A86A-83BD53631910}"_cry_guid;
		return id;
	}

	void Update();

public:

	/**
	Player cameras generally need to follow an actor. This allows us to switch which entity represents the actor that
	the camera is following.

	\param	entityID	If non-null, the entity.
	*/
	void AttachToEntity(EntityId entityID);


	/**
	Gets current camera mode.

	\return	The current camera mode.
	*/
	ECameraMode GetCameraMode();


	/**
	Sets camera mode.

	\param	mode  	The mode.
	\param	reason	The reason.
	*/
	void SetCameraMode(ECameraMode mode, const char* reason);


	/**
	Sets the present camera mode to whatever the last camera mode was. This should work for simple situations where
	the camera is switched to a new view and needs to be switched back again after use.

	\param	reason The reason.
	**/
	void SetLastCameraMode();


	/**
	Gets the currently active camera.

	\return	null if it fails, else the camera.
	*/
	ICameraComponent* GetCamera() const;


	/**
	Gets whether this instance is in third person or not.

	\return	Whether this instance is in third person or not.
	*/
	bool IsThirdPerson() const;


	/**
	Toggle between the first and third person camera modes, if either is selected.
	*/
	void ToggleThirdPerson();


	// ***
	// *** CCameraManagerComponent
	// ***

	/**
	Gets view offset.

	\return The view offset.
	**/
	Vec3 GetViewOffset();

	/**	Camera debug actions. **/
	bool OnActionCameraShiftUp(EntityId entityId, const ActionId& actionId, int activationMode, float value);
	bool OnActionCameraShiftDown(EntityId entityId, const ActionId& actionId, int activationMode, float value);
	bool OnActionCameraShiftLeft(EntityId entityId, const ActionId& actionId, int activationMode, float value);
	bool OnActionCameraShiftRight(EntityId entityId, const ActionId& actionId, int activationMode, float value);
	bool OnActionCameraShiftForward(EntityId entityId, const ActionId& actionId, int activationMode, float value);
	bool OnActionCameraShiftBackward(EntityId entityId, const ActionId& actionId, int activationMode, float value);


private:
	/**	An array large enough to hold one of each defined camera mode. **/
	ICameraComponent* m_cameraModes [ECameraMode::eCameraMode_Last];

	/** The current camera mode. */
	ECameraMode m_cameraMode { ECameraMode::eCameraMode_NoCamera };

	/** The last camera mode. */
	ECameraMode m_lastCameraMode { ECameraMode::eCameraMode_NoCamera };

	/** The amount each offset tweak will adjust the camera by. **/
	const float adjustmentAmount { 0.025f };

	/** Provides a last moment offset for the view - useful for debugging and player controller extra camera movement. **/
	Vec3 m_interactiveViewOffset { ZERO };

	/** The player. */
	CPlayerComponent* m_pPlayer { nullptr };
};
}