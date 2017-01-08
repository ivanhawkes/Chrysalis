#pragma once

#include "ICameraComponent.h"
#include <IViewSystem.h>
#include <Player/Camera/CameraManagerComponent.h>


/**
A camera suitable for use with action RPG style games.

\sa CGameObjectExtensionHelper&lt;CActionRPGCameraComponent, IGameObjectExtension&gt;
\sa IActionListener
**/
class CActionRPGCameraComponent : public CGameObjectExtensionHelper <CActionRPGCameraComponent, ICameraComponent>, public IGameObjectView
//class CActionRPGCameraComponent : public ICameraComponent, public IGameObjectView
{
	CRY_ENTITY_COMPONENT_INTERFACE_AND_CLASS(CActionRPGCameraComponent, "ActionRPGCamera", 0xEF4577772A784F7E, 0xAC889E99FC0DE6B9)

public:
	// IEntityComponent
	void Initialize() override;
	void ProcessEvent(SEntityEvent& event) override;
	uint64 GetEventMask() const { return m_EventMask; }
	void OnShutDown() override;
	// ~IEntityComponent

	virtual bool Init(IGameObject* pGameObject) override { SetGameObject(pGameObject); return true; }
	void PostInit(IGameObject * pGameObject) override;

	// TODO: Refactor when 5.4 is released.
	//void Update2();
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


	// ***
	// *** CActionRPGCameraComponent
	// ***

public:

	struct SExternalCVars
	{
		int m_debug;
		float m_pitchMin;
		float m_pitchMax;
		float m_targetDistance;
		float m_reversePitchTilt;
		float m_slerpSpeed;
		float m_zoomMin;
		float m_zoomMax;
		float m_zoomStep;
		float m_zoomSpeed;
		ICVar* m_viewPositionOffset;
		ICVar* m_aimPositionOffset;
	};
	const SExternalCVars &GetCVars() const;


	CActionRPGCameraComponent() { ResetCamera(); }
	virtual ~CActionRPGCameraComponent() {};

	/** Resets the results of all player based camera movements back to their defaults. */
	virtual void ResetCamera();

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

private:

	/** The view camera for this instance. */
	IView* m_pView { nullptr };

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
	*/
	float m_zoom;

	/** A delta value (degrees) to apply to the camera's initial calculated pitch. */
	float m_viewPitch;

	/** A delta value (degrees) to apply to the camera's initial calculated yaw. */
	float m_viewYaw;

	/** Provides a way to avoid updates when they are not required. **/
	uint64 m_EventMask { 0L };
};