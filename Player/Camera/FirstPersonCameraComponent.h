#pragma once

#include "ICameraComponent.h"
#include <IViewSystem.h>
#include <Player/Camera/CameraManagerComponent.h>


/**
An camera suitable for use in games requiring a first person view.

\sa CGameObjectExtensionHelper&lt;CFirstPersonCameraComponent, IGameObjectExtension&gt;
\sa IGameObjectView
**/
class CFirstPersonCameraComponent : public CGameObjectExtensionHelper <CFirstPersonCameraComponent, ICameraComponent>, public IGameObjectView
//class CFirstPersonCameraComponent : public ICameraComponent, public IGameObjectView
{
	CRY_ENTITY_COMPONENT_INTERFACE_AND_CLASS(CFirstPersonCameraComponent, "FirstPersonCamera", 0xD61307A7547B42CF, 0xA3BDBD9BC13A0064)

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

	\param	entityID	If non-null, the entity.
	*/
	void AttachToEntity(EntityId entityId) override { m_targetEntityID = entityId; }

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


	CFirstPersonCameraComponent() { ResetCamera(); }
	virtual ~CFirstPersonCameraComponent() {};

	virtual void ResetCamera() { m_viewPitch = (DEG2RAD(GetCVars().m_pitchMax) + DEG2RAD(GetCVars().m_pitchMin)) / 2; }


private:
	/** If our entity has a camera manager, we store a pointer to it here. **/
	CCameraManagerComponent* m_pCameraManager { nullptr };

	/** Identifier for the entity which this camera is targeted towards. */
	EntityId m_targetEntityID { INVALID_ENTITYID };

	/** The view camera for this instance. */
	IView* m_pView { nullptr };

	/** A delta value (degrees) to apply to the camera's initial calculated pitch. */
	float m_viewPitch;

	/** Provides a way to avoid updates when they are not required. **/
	uint64 m_EventMask { 0L };
};