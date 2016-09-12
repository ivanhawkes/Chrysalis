#pragma once

#include "ICameraComponent.h"
#include <IViewSystem.h>
#include <Player/Camera/CameraManagerComponent.h>


/**
An camera suitable for use in games requiring a first person view.

\sa CGameObjectExtensionHelper&lt;CFirstPersonCameraComponent, IGameObjectExtension&gt;
\sa IGameObjectView
**/
class CFirstPersonCameraComponent : public CGameObjectExtensionHelper <CFirstPersonCameraComponent, ICameraComponent>,public IGameObjectView
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
	// *** ICameraComponent
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

	struct SExternalCVars
	{
		int m_debug;
		float m_pitchMin;
		float m_pitchMax;
	};
	const SExternalCVars &GetCVars() const;


	CFirstPersonCameraComponent();

	virtual ~CFirstPersonCameraComponent();
	virtual void ResetCamera();


private:
	/** If our entity has a camera manager, we store a pointer to it here. **/
	ICameraManagerComponent* m_pCameraManager { nullptr };

	/** Identifier for the entity which this camera is targeted towards. */
	EntityId m_targetEntityID { INVALID_ENTITYID };

	/** The view camera for this instance. */
	IView* m_pView { nullptr };

	/** A delta value (degrees) to apply to the camera's initial calculated pitch. */
	float m_viewPitch;
};