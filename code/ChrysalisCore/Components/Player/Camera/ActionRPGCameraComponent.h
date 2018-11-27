#pragma once

#include "ICameraComponent.h"
#include <CrySystem/VR/IHMDDevice.h>
#include <CrySystem/VR/IHMDManager.h>
#include "../Camera/CameraManagerComponent.h"


namespace Chrysalis
{
/** A camera suitable for use with action RPG style games. */
class CActionRPGCameraComponent
	: public ICameraComponent
{
protected:
	friend CChrysalisCorePlugin;
	static void Register(Schematyc::CEnvRegistrationScope& componentScope);

	// IEntityComponent
	void Initialize() override;
	void ProcessEvent(const SEntityEvent& event) override;
	Cry::Entity::EntityEventMask GetEventMask() const override { return m_EventMask; }
	void OnShutDown() override;
	// ~IEntityComponent

public:
	CActionRPGCameraComponent() {}
	virtual ~CActionRPGCameraComponent() {}

	static void ReflectType(Schematyc::CTypeDesc<CActionRPGCameraComponent>& desc);

	static CryGUID& IID()
	{
		static CryGUID id = "{BA667E39-D20C-429A-9DF1-9A268EBD7BCA}"_cry_guid;
		return id;
	}

	// ***
	// *** ICameraComponent
	// ***


	/**
	Switch which entity the camera will treat as it's attachment.

	\param	entityID If non-null, the entity.
	**/
	virtual void AttachToEntity(EntityId entityID);


	/** Executes the activate action. */
	void OnActivate() override;

	/** Executes the deactivate action. */
	void OnDeactivate() override;

	/** Is the view defined by this camera in first person? */
	bool IsViewFirstPerson() const override { return m_isFirstPerson; };


	// ***
	// *** CActionRPGCameraComponent
	// ***

public:
	/** Resets the results of all player based camera movements back to their defaults. */
	virtual void ResetCamera();


	/**
	Determines the position for camera to aim at on the target entity. It relies on the target being an actor for
	accurate results, otherwise the position will be based on the average height for a person.
	
	\param [in,out]	pEntity If non-null, the entity.
	
	\return The target aim position.
	**/
	Vec3 GetTargetAimPosition(IEntity* const pEntity);


	/**
	Performs a very simple collision detection. The camera view position is updated to the best viewing location
	based on the results of the raycasting.
	
	\param 		   	Goal		   The goal.
	\param [in,out]	CameraPosition The camera position.
	
	\return true if it succeeds, false if it fails.
	**/
	bool CollisionDetection(const Vec3& Goal, Vec3& CameraPosition);

private:

	void Update();
	void UpdateZoom();
	void UpdateFirstPerson();
	void UpdateThirdPerson();

	/**	Toggle between the first and third person camera modes, if either is selected. */
	void ToggleFirstPerson();

	/** If our entity has a camera manager, we store a pointer to it here. **/
	CCameraManagerComponent* m_pCameraManager { nullptr };

	/** Identifier for the entity which this camera is targeted towards. */
	EntityId m_targetEntityID { INVALID_ENTITYID };

	/** Position of the camera during the last update. */
	Vec3 m_vecLastPosition { ZERO };

	/** Rotation of the camera during the last update. */
	Quat m_quatLastTargetRotation { IDENTITY };

	/** Initial rotation of the camera in reference to the entity it is attached onto. */
	Quat m_quatTargetRotation { IDENTITY };

	/** The last known position of the camera target. */
	Vec3 m_vecLastTargetPosition { ZERO };

	/** The last known position of the camera aim target. */
	Vec3 m_vecLastTargetAimPosition { ZERO };

	/**
	The last rotation value for the view goal. This is used in the SLERP calculations for camera smoothing through
	rotations.
	*/
	Quat m_quatLastViewRotation { IDENTITY };

	/**
	Set to true when you want to avoid using interpolation for the update next frame. There are times when we don't
	have a reasonable value for the last camera position / rotation and this allows us to get around those awkward
	moments.
	*/
	bool m_skipInterpolation { true };

	/** The zoom goal. */
	float m_zoomGoal;

	/** The last zoom goal. Need to track where we wanted to be, from frame to frame, or zoom interpolate will fail. */
	float m_lastZoomGoal;

	/**
	Determine the level of zoom on the third person camera. Sensible values are from 0.0f up to 100.0f, although it
	is capable of zooming beyond that.
	**/
	float m_zoom;

	/** A delta value (degrees) to apply to the camera's initial calculated pitch. */
	float m_viewPitch;

	/** A delta value (degrees) to apply to the camera's initial calculated yaw. */
	float m_viewYaw;

	/** Provides a way to avoid updates when they are not required. **/
	Cry::Entity::EntityEventMask m_EventMask;

	/** Is the camera view in first person mode? **/
	bool m_isFirstPerson { true };
};
}