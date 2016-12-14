#pragma once

#include "ICameraComponent.h"


struct ICameraComponent;


struct ICameraManagerComponent : public ISimpleExtension
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
	
	\return null if it fails, else the camera.
	**/
	virtual ICameraComponent* GetCamera() const = 0;


	/**
	The camera manager may supply an extra offset in local space for camera adjustments to the view.
	
	\return The view offset.
	**/
	virtual Vec3 GetViewOffset() = 0;


	/**
	Gets whether this instance is in third person or not.

	\return	Whether this instance is in third person or not.
	*/
	virtual bool IsThirdPerson() const = 0;


	/**
	Toggles third person mode of this instance (typically meant to toggle between third person and first person models and cameras).
	*/
	virtual void ToggleThirdPerson() = 0;

	/**	Camera debug actions. **/
	virtual bool OnActionCameraShiftUp(EntityId entityId, const ActionId& actionId, int activationMode, float value) = 0;
	virtual bool OnActionCameraShiftDown(EntityId entityId, const ActionId& actionId, int activationMode, float value) = 0;
	virtual bool OnActionCameraShiftLeft(EntityId entityId, const ActionId& actionId, int activationMode, float value) = 0;
	virtual bool OnActionCameraShiftRight(EntityId entityId, const ActionId& actionId, int activationMode, float value) = 0;
	virtual bool OnActionCameraShiftForward(EntityId entityId, const ActionId& actionId, int activationMode, float value) = 0;
	virtual bool OnActionCameraShiftBackward(EntityId entityId, const ActionId& actionId, int activationMode, float value) = 0;
};