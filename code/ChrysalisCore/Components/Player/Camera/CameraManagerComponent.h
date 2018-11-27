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
#include <DefaultComponents/Input/InputComponent.h>


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
	void ProcessEvent(const SEntityEvent& event) override;
	Cry::Entity::EntityEventMask GetEventMask() const override { return EventToMask(EEntityEvent::Update); }
	// ~IEntityComponent

	void RegisterActionMaps();

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
	Gets whether this instance is in first person or not.

	\return	Whether this instance is in first person or not.
	*/
	bool IsViewFirstPerson() const;


	/**
	Gets number of times the player has requested a change in zoom level since the last frame. Cameras can query this
	value and use it to adjust their zoom. Value is set in stone on PrePhysicsUpdate () so it can be consistently queried
	during the Update() routine. It's provided as a float, even though currently implementation is integral steps. This
	will give us more fine control if needed later.
	
	\return The zoom delta.
	**/
	float GetZoomDelta() { return m_lastZoomDelta; };


	/**
	Gets view offset.

	\return The view offset.
	**/
	Vec3 GetViewOffset();

	/**	Camera debug actions. **/
	bool OnActionCameraShiftUp(int activationMode, float value);
	bool OnActionCameraShiftDown(int activationMode, float value);
	bool OnActionCameraShiftLeft(int activationMode, float value);
	bool OnActionCameraShiftRight(int activationMode, float value);
	bool OnActionCameraShiftForward(int activationMode, float value);
	bool OnActionCameraShiftBackward(int activationMode, float value);


private:
	/** The player. */
	CPlayerComponent* m_pPlayer { nullptr };

	/** The input component */
	Cry::DefaultComponents::CInputComponent* m_pInputComponent { nullptr };

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

	/**	Zoom delta. A value that indicates how much they wish to zoom in (negative values) or out (positive values). **/
	float m_zoomDelta { 0.0f };

	/**	Last zoom delta. **/
	float m_lastZoomDelta { 0.0f };
};
}