#pragma once

#include <IGameObject.h>
#include "ICamera.h"

struct ICamera;


struct ICameraManager : public IGameObjectExtension
{
	/**
	Player cameras generally need to follow an actor. This allows us to switch which entity represents the actor that
	the camera is following. The presently in-use camera is instructed to follow the supplied entity.
	
	\param	entityID If non-null, the entity.
	**/
	virtual void AttachToEntity(EntityId entityID) = 0;


	/**
	Gets current camera mode.
	
	\return The current camera mode.
	**/
	virtual ECameraMode GetCameraMode() = 0;


	/**
	Sets camera mode.
	
	\param	mode   The mode.
	\param	reason The reason.
	**/
	virtual void SetCameraMode(ECameraMode mode, const char* reason) = 0;


	/**
	Gets the currently active camera.

	\return	null if it fails, else the camera.
	*/
	virtual ICamera* GetCamera() const = 0;
};