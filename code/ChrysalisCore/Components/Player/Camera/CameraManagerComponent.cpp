#include <StdAfx.h>

#include "CameraManagerComponent.h"
#include <CryCore/StaticInstanceList.h>
#include "CrySchematyc/Env/Elements/EnvComponent.h"
#include "CrySchematyc/Env/IEnvRegistrar.h"
#include "ActionRPGCameraComponent.h"
#include "ExamineCameraComponent.h"
#include "FirstPersonCameraComponent.h"
#include <Utility/StringConversions.h>
#include <Console/CVars.h>


namespace Chrysalis
{
static void RegisterCameraManagerComponent(Schematyc::IEnvRegistrar& registrar)
{
	Schematyc::CEnvRegistrationScope scope = registrar.Scope(IEntity::GetEntityScopeGUID());
	{
		Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(CCameraManagerComponent));
		// Functions
		{
		}
	}
}


void CCameraManagerComponent::ReflectType(Schematyc::CTypeDesc<CCameraManagerComponent>& desc)
{
	desc.SetGUID(CCameraManagerComponent::IID());
	desc.SetEditorCategory("Hidden");
	desc.SetLabel("Camera Manager");
	desc.SetDescription("No description.");
	desc.SetIcon("icons:ObjectTypes/light.ico");
	desc.SetComponentFlags({IEntityComponent::EFlags::Singleton, IEntityComponent::EFlags::Socket, IEntityComponent::EFlags::Attach, IEntityComponent::EFlags::ClientOnly});
}


// ***
// *** IGameObjectExtension
// ***


void CCameraManagerComponent::Initialize()
{
	// Query for the player that owns this extension.
	m_pPlayer = m_pEntity->GetComponent<CPlayerComponent>();

	// #HACK: #TODO: Is this right? We only want to register action maps on the local client.
	if (GetEntityId() == gEnv->pGameFramework->GetClientActorId())
		RegisterActionMaps();

	// First person camera.
	m_cameraModes[ECameraMode::eCameraMode_FirstPerson] = m_pEntity->CreateComponent<CFirstPersonCameraComponent>();

	// Action RPG Camera.
	m_cameraModes[ECameraMode::eCameraMode_ActionRpg] = m_pEntity->CreateComponent<CActionRPGCameraComponent>();

	// Examine entity camera.
	m_cameraModes[ECameraMode::eCameraMode_Examine] = m_pEntity->CreateComponent<CExamineCameraComponent>();

	// Select the initial camera based on their cvar setting.
	SetCameraMode((ECameraMode)g_cvars.m_cameraManagerDefaultCamera, "Initial selection of camera.");
}


void CCameraManagerComponent::ProcessEvent(const SEntityEvent& event)
{
	switch (event.event)
	{
		case EEntityEvent::Update:
			Update();
			break;
	}
}


void CCameraManagerComponent::Update()
{
	// Handle zoom level changes, result is stored for query by cameras on Update.
	m_lastZoomDelta = m_zoomDelta;
	m_zoomDelta = 0.0f;
}


// ***
// *** CCameraManagerComponent
// ***


void CCameraManagerComponent::AttachToEntity(EntityId entityID)
{
	// Let all the cameras know we switch entities. Only a few will care, but it's safer to just let them all handle
	// it. An alternative would be to perform an attach when switching cameras. This is good enough for now. 
	for (unsigned int i = 0; i < ECameraMode::eCameraMode_Last; ++i)
	{
		if (m_cameraModes[i])
			m_cameraModes[i]->AttachToEntity(entityID);
	}
}


ECameraMode CCameraManagerComponent::GetCameraMode()
{
	return m_cameraMode;
}


void CCameraManagerComponent::SetCameraMode(ECameraMode mode, const char* reason)
{
	if (m_cameraMode != mode)
	{
		// Tell the previous camera it's no longer in use.
		if (m_cameraMode != ECameraMode::eCameraMode_NoCamera)
			m_cameraModes[m_cameraMode]->OnDeactivate();

		// Tell the new camera it is entering usage.
		if (mode != ECameraMode::eCameraMode_NoCamera)
			m_cameraModes[mode]->OnActivate();

		// Track the previous camera mode, in case we want to switch back to it.
		m_lastCameraMode = m_cameraMode;

		// Set the new mode.
		m_cameraMode = mode;

		// Debug.
		CryLogAlways("SetCameraMode from %d to %d, reason - %s", m_lastCameraMode, m_cameraMode, reason);
	}
	else
	{
		// Debug.
		CryLogAlways("SetCameraMode from %d to %d, reason - %s (Skipped)", m_lastCameraMode, m_cameraMode, reason);
	}
}


void CCameraManagerComponent::SetLastCameraMode()
{
	if (m_lastCameraMode != m_cameraMode)
	{
		SetCameraMode(m_lastCameraMode, "Set mode to the last camera mode.");
	}
}


ICameraComponent* CCameraManagerComponent::GetCamera() const
{
	return m_cameraModes[m_cameraMode];
}


bool CCameraManagerComponent::IsViewFirstPerson() const
{
	return GetCamera()->IsViewFirstPerson();
}


CCameraManagerComponent::CCameraManagerComponent()
{
	// We'll take an initial value for the debug view offset from cvars.
	m_interactiveViewOffset = Vec3FromString(g_cvars.m_actionRPGCameraViewPositionOffset->GetString());

	// Start with a known clean state.
	memset(m_cameraModes, 0, sizeof(m_cameraModes));
}


void CCameraManagerComponent::RegisterActionMaps()
{
	// Get the input component, wraps access to action mapping so we can easily get callbacks when inputs are triggered.
	m_pInputComponent = m_pEntity->GetOrCreateComponent<Cry::DefaultComponents::CInputComponent>();

	// Zoom handlers.
	m_pInputComponent->RegisterAction("camera", "tpv_zoom_in", [this](int activationMode, float value) { m_zoomDelta -= 1.0f; });
	m_pInputComponent->BindAction("camera", "tpv_zoom_in", eAID_KeyboardMouse, EKeyId::eKI_MouseWheelUp);
	m_pInputComponent->RegisterAction("camera", "tpv_zoom_out", [this](int activationMode, float value) { m_zoomDelta += 1.0f; });
	m_pInputComponent->BindAction("camera", "tpv_zoom_out", eAID_KeyboardMouse, EKeyId::eKI_MouseWheelDown);

	// Camera movements.
	m_pInputComponent->RegisterAction("camera", "camera_shift_up", [this](int activationMode, float value) { OnActionCameraShiftUp(activationMode, value); });
	m_pInputComponent->BindAction("camera", "camera_shift_up", eAID_KeyboardMouse, EKeyId::eKI_PgUp);
	m_pInputComponent->RegisterAction("camera", "camera_shift_down", [this](int activationMode, float value) { OnActionCameraShiftDown(activationMode, value); });
	m_pInputComponent->BindAction("camera", "camera_shift_down", eAID_KeyboardMouse, EKeyId::eKI_PgDn);
	m_pInputComponent->RegisterAction("camera", "camera_shift_left", [this](int activationMode, float value) { OnActionCameraShiftLeft(activationMode, value); });
	m_pInputComponent->BindAction("camera", "camera_shift_left", eAID_KeyboardMouse, EKeyId::eKI_Left);
	m_pInputComponent->RegisterAction("camera", "camera_shift_right", [this](int activationMode, float value) { OnActionCameraShiftRight(activationMode, value); });
	m_pInputComponent->BindAction("camera", "camera_shift_right", eAID_KeyboardMouse, EKeyId::eKI_Right);
	m_pInputComponent->RegisterAction("camera", "camera_shift_forward", [this](int activationMode, float value) { OnActionCameraShiftForward(activationMode, value); });
	m_pInputComponent->BindAction("camera", "camera_shift_forward", eAID_KeyboardMouse, EKeyId::eKI_Up);
	m_pInputComponent->RegisterAction("camera", "camera_shift_backward", [this](int activationMode, float value) { OnActionCameraShiftBackward(activationMode, value); });
	m_pInputComponent->BindAction("camera", "camera_shift_backward", eAID_KeyboardMouse, EKeyId::eKI_Down);
}


Vec3 CCameraManagerComponent::GetViewOffset()
{
	return Vec3FromString(g_cvars.m_actionRPGCameraViewPositionOffset->GetString()) + m_interactiveViewOffset;
}


bool CCameraManagerComponent::OnActionCameraShiftUp(int activationMode, float value)
{
	if (activationMode & (eAAM_OnPress | eAAM_OnHold))
		m_interactiveViewOffset += Vec3(0.0f, 0.0f, adjustmentAmount);

	return false;
}


bool CCameraManagerComponent::OnActionCameraShiftDown(int activationMode, float value)
{
	if (activationMode & (eAAM_OnPress | eAAM_OnHold))
		m_interactiveViewOffset += Vec3(0.0f, 0.0f, -adjustmentAmount);

	return false;
}


bool CCameraManagerComponent::OnActionCameraShiftLeft(int activationMode, float value)
{
	if (activationMode & (eAAM_OnPress | eAAM_OnHold))
		m_interactiveViewOffset += Vec3(-adjustmentAmount, 0.0f, 0.0f);

	return false;
}


bool CCameraManagerComponent::OnActionCameraShiftRight(int activationMode, float value)
{
	if (activationMode & (eAAM_OnPress | eAAM_OnHold))
		m_interactiveViewOffset += Vec3(adjustmentAmount, 0.0f, 0.0f);

	return false;
}


bool CCameraManagerComponent::OnActionCameraShiftForward(int activationMode, float value)
{
	if (activationMode & (eAAM_OnPress | eAAM_OnHold))
		m_interactiveViewOffset += Vec3(0.0f, adjustmentAmount, 0.0f);

	return false;
}


bool CCameraManagerComponent::OnActionCameraShiftBackward(int activationMode, float value)
{
	if (activationMode & (eAAM_OnPress | eAAM_OnHold))
		m_interactiveViewOffset += Vec3(0.0f, -adjustmentAmount, 0.0f);

	return false;
}

CRY_STATIC_AUTO_REGISTER_FUNCTION(&RegisterCameraManagerComponent)
}