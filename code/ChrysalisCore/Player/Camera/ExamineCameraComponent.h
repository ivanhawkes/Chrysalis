#pragma once

#include "ICameraComponent.h"
#include <IViewSystem.h>
#include <Player/Camera/CameraManagerComponent.h>


/**
An camera used to get up close to entities allowing the player to examine them more closely than might be
possible with the default view.

\sa CGameObjectExtensionHelper&lt;CExamineCameraComponent, IGameObjectExtension&gt;
\sa IGameObjectView.
**/
class CExamineCameraComponent : public CGameObjectExtensionHelper <CExamineCameraComponent, ICameraComponent>, public IGameObjectView
//class CExamineCameraComponent : public ICameraComponent, public IGameObjectView
{
	CRY_ENTITY_COMPONENT_INTERFACE_AND_CLASS(CExamineCameraComponent, "ExamineCamera", 0xEF0DFEC918704C03, 0xA78FD340D8212547)

public:
	// IEntityComponent
	void Initialize() override;
	void ProcessEvent(SEntityEvent& event) override;
	uint64 GetEventMask() const { return m_EventMask; }
	void OnShutDown() override;
	// ~IEntityComponent

	virtual bool Init(IGameObject* pGameObject) override { SetGameObject(pGameObject); return true; }
	void PostInit(IGameObject * pGameObject) override;

	// #TODO: Refactor when 5.4 is released.
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
	void AttachToEntity(EntityId entityId) override;

	/** Executes the activate action. */
	void OnActivate() override;

	/** Executes the deactivate action. */
	void OnDeactivate() override;


	// ***
	// *** CExamineRPGCamera
	// ***

public:

	struct SExternalCVars
	{
		int m_debug;
		float m_yawMin;
		float m_yawMax;
		float m_pitchMin;
		float m_pitchMax;
	};
	const SExternalCVars &GetCVars() const;


	CExamineCameraComponent() { ResetCamera(); }
	virtual ~CExamineCameraComponent() {};

	virtual void ResetCamera();


private:
	/** If our entity has a camera manager, we store a pointer to it here. **/
	CCameraManagerComponent* m_pCameraManager { nullptr };

	/** Identifier for the entity which this camera is targeted towards. */
	EntityId m_targetEntityID { INVALID_ENTITYID };

	/** The view camera for this instance. */
	IView* m_pView { nullptr };

	/** A delta value (degrees) to apply to the camera's initial calculated pitch. */
	float m_viewPitch;

	/** A delta value (degrees) to apply to the camera's initial calculated yaw. */
	float m_viewYaw;

	/** Provides a way to avoid updates when they are not required. **/
	uint64 m_EventMask { 0L };
};