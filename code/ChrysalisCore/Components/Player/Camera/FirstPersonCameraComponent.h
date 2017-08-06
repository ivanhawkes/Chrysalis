#pragma once

#include "ICameraComponent.h"
#include <CrySystem/VR/IHMDDevice.h>
#include <CrySystem/VR/IHMDManager.h>
#include "../Camera/CameraManagerComponent.h"
#include <Console/CVars.h>


namespace Chrysalis
{
/** An camera suitable for use in games requiring a first person view. */
class CFirstPersonCameraComponent
	: public ICameraComponent
{
protected:
	friend CChrysalisCorePlugin;
	static void Register(Schematyc::CEnvRegistrationScope& componentScope);

	// IEntityComponent
	void Initialize() override;
	void ProcessEvent(SEntityEvent& event) override;
	uint64 GetEventMask() const { return m_EventMask; }
	void OnShutDown() override;
	// ~IEntityComponent

	// IAsyncCameraCallback
	virtual bool OnAsyncCameraCallback(const HmdTrackingState& state, IHmdDevice::AsyncCameraContext& context) final;
	// ~IAsyncCameraCallback

	// IEntityEventListener
	virtual void OnEntityEvent(IEntity* pEntity, SEntityEvent& event) final;
	// ~IEntityEventListener

public:
	CFirstPersonCameraComponent() { ResetCamera(); }
	virtual ~CFirstPersonCameraComponent() {}

	static void ReflectType(Schematyc::CTypeDesc<CFirstPersonCameraComponent>& desc);

	static CryGUID& IID()
	{
		static CryGUID id = "{9558F62B-C8FA-4161-9B6F-B515FA921C20}"_cry_guid;
		return id;
	}

	void Update();


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
	virtual void ResetCamera() { m_viewPitch = (DEG2RAD(g_cvars.m_firstPersonCameraPitchMax) + DEG2RAD(g_cvars.m_firstPersonCameraPitchMin)) / 2; }


private:
	/** If our entity has a camera manager, we store a pointer to it here. **/
	CCameraManagerComponent* m_pCameraManager { nullptr };

	/** Identifier for the entity which this camera is targeted towards. */
	EntityId m_targetEntityID { INVALID_ENTITYID };

	/** A delta value (degrees) to apply to the camera's initial calculated pitch. */
	float m_viewPitch;

	/** Provides a way to avoid updates when they are not required. **/
	uint64 m_EventMask { 0L };
};
}