#pragma once

#include "ICamera.h"
#include <IViewSystem.h>
#include <Camera/CameraManager.h>


/**
An first person camera suitable for use with games using a first person view.

\sa CGameObjectExtensionHelper&lt;CFirstPersonCamera, IGameObjectExtension&gt;
\sa IGameObjectView
**/
class CFirstPersonCamera : public CGameObjectExtensionHelper <CFirstPersonCamera, ICamera>,public IGameObjectView
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

	\param	entityID	If non-null, the entity.
	*/
	void AttachToEntity(EntityId entityID) override;

	/** Executes the activate action. */
	void OnActivate() override;

	/** Executes the deactivate action. */
	void OnDeactivate() override;


	// ***
	// *** CFirstPersonRPGCamera
	// ***

public:

	/** This instance's default constructor. */
	CFirstPersonCamera();

	/** This instance's default destructor. */
	virtual ~CFirstPersonCamera();

	/** Resets the results of all player based camera movements back to their defaults. */
	virtual void ResetCamera();


private:
	/** If our entity has a camera manager, we store a pointer to it here. **/
	ICameraManager* m_pCameraManager { nullptr };

	/** Identifier for the entity which this camera is targeted towards. */
	EntityId m_targetEntityID { INVALID_ENTITYID };

	/** The view camera for this instance. */
	IView* m_pView { nullptr };

	/** A delta value (degrees) to apply to the camera's initial calculated pitch. */
	float m_viewPitch;

	/** Minimum pitch delta (radians). */
	float m_pitchMin { DEG2RAD(-85.0f) };

	/** Maximum pitch delta (radians). */
	float m_pitchMax { DEG2RAD(85.0f) };
};