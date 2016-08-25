#pragma once

#include "ICamera.h"
#include <IViewSystem.h>
#include <Camera/CameraManager.h>


/**
An first person camera suitable for use with games using a first person view.

\sa	CGameObjectExtensionHelper<CActionRPGCamera, IGameObjectExtension>
\sa	IActionListener
*/
class CActionRPGCamera : public CGameObjectExtensionHelper <CActionRPGCamera, ICamera>, public IGameObjectView
{
public:

	// ***
	// *** IGameObjectExtension
	// ***

	void PostInit(IGameObject * pGameObject) override;
	bool ReloadExtension(IGameObject * pGameObject, const SEntitySpawnParams &params) override;
	void Release() override;
	void Update(SEntityUpdateContext& ctx, int updateSlot) override;


	// ***
	// *** IGameObjectView
	// ***


	/**
	Called when the view should provide an update of it's current state.
	
	\param	params A structure with members that are used to control view parameters.
	**/
	void UpdateView(SViewParams& params) override;


	/**
	Called when the view should provide a post update of it's current state.
	
	\param	params A variable-length parameters list containing view parameters.
	**/
	void PostUpdateView(SViewParams& params) override {};


	// ***
	// *** ICamera
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


	// ***
	// *** CActionRPGCamera
	// ***

public:

	/** This instance's default constructor. */
	CActionRPGCamera();


	/** This instance's default destructor. */
	virtual ~CActionRPGCamera();


	/** Resets the results of all player based camera movements back to their defaults. */
	virtual void ResetCamera();
	
private:

	/** The view camera for this instance. */
	IView* m_pView { nullptr };

	/** If our entity has a camera manager, we store a pointer to it here. **/
	ICameraManager* m_pCameraManager { nullptr };
	
	/** Identifier for the entity which this camera is targeted towards. */
	EntityId m_targetEntityID { INVALID_ENTITYID };

	/** Preferred distance in metres from the view camera to the target. */
	float m_targetDistance { 5.0f };

	/** Position of the camera during the last update. */
	Vec3 m_vecLastPosition { ZERO };

	/** Rotation of the camera during the last update. */
	Quat m_quatLastTargetRotation { IDENTITY };

	/** Initial rotation of the camera in reference to the entity it is attached onto. */
	Quat m_quatTargetRotation { IDENTITY };

	/**
	A factor that applies pitch in the direction the camera is facing. The effect of this is to tilt it towards or
	away from the orbit centre.
	*/
	float m_reversePitchTilt { 0.1f };
//	float m_reversePitchTilt { 0.0f };

	/**
	A translation vector which is applied after the camera is initially positioned. This provides for 'over the
	shoulder' views of the target actor.
	*/
	Vec3 m_viewPositionOffset { ZERO };

	/**
	A translation vector which is applied after the camera is initially positioned. This provides for 'over the
	shoulder' views of the target actor.
	*/
	Vec3 m_aimPositionOffset { 0.45f, -0.5f, 0.0f };
//	Vec3 m_aimPositionOffset { 0.0f, -0.0f, 0.0f };

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
	The slerp speed. The speed at which we apply spherical linear interpolation to the camera rotations.
	*/
	float m_slerpSpeed { 20.0f };

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
	*/
	float m_zoom;

	/** The minimum value for camera zoom. */
	float m_zoomMin { 0.2f };

	/** The maximum value for camera zoom. */
	float m_zoomMax { 1.0f };

	/**
	Each zoom event in or out will alter the zoom factor, m_zoom, by this amount. Use lower values for more steps and
	higher values to zoom in / out faster with less steps.
	*/
	float m_zoomStep { 0.08f };

	/**
	When the zoom changes we interpolate between it's last value and the goal value. This provides for smoother
	movement on camera zooms. Higher values will interpolate faster than lower values.
	*/
	float m_zoomSpeed { 10.0f };

	/** A delta value (degrees) to apply to the camera's initial calculated pitch. */
	float m_viewPitch;

	/** Minimum pitch delta (radians). */
	float m_pitchMin { DEG2RAD(-85.0f) };

	/** Maximum pitch delta (radians). */
	float m_pitchMax { DEG2RAD(85.0f) };

	/** A delta value (degrees) to apply to the camera's initial calculated yaw. */
	float m_viewYaw;

	/** Minimum yaw delta (radians). */
	float m_yawMin { DEG2RAD(-180.0f) };

	/** Maximum yaw delta (radians). */
	float m_yawMax { DEG2RAD(180.0f) };


	// ***
	// *** Camera movement and updates.
	// ***


	/**
	Determines the position for camera to aim at on the target entity. It relies on the target being an actor for
	accurate results, otherwise the position will be based on the average height for a person.
	
	\param [in,out]	vecTargetPosition The target's position will be returned in this parameter.
	
	\param [in,out]	pEntity If non-null, the entity.
	
	\return The target aim position.
	**/
	Vec3 GetTargetAimPosition(IEntity* const pEntity);


	/**
	Performs a very simple collision detection. The camera view position is updated to the best viewing location
	based on the results of the raycasting.
	
	\param	Goal				   The goal.
	\param [in,out]	CameraPosition The camera position.
	
	\return true if it succeeds, false if it fails.
	**/
	bool CollisionDetection(const Vec3& Goal, Vec3& CameraPosition);
};