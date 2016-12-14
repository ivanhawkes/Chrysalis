#include <StdAfx.h>

#include "CameraManagerComponent.h"
#include "ActionRPGCameraComponent.h"
#include "FirstPersonCameraComponent.h"
#include <Player/Player.h>
#include <Utility/StringConversions.h>


class CCameraManagerRegistrator
	: public IEntityRegistrator
	, public CCameraManagerComponent::SExternalCVars
{
	virtual void Register() override
	{
		CGameFactory::RegisterGameObjectExtension<CCameraManagerComponent>("CameraManager");

		REGISTER_CVAR2("camera_manager_IsThirdPerson", &m_isThirdPerson, 0, VF_CHEAT, "Is the player camera in first person or third person mode? 0 = first person, any else third person.");

		m_debugViewOffset = REGISTER_STRING("camera_manager_DebugViewOffset", "0, 0, 0", VF_CHEAT, "A translation vector which is applied after the camera is initially positioned.");
	}
};

CCameraManagerRegistrator g_cameraManagerRegistrator;


// ***
// *** IGameObjectExtension
// ***


void CCameraManagerComponent::PostInit(IGameObject * pGameObject)
{
	// Allow this instance to be updated every frame.
	pGameObject->EnableUpdateSlot(this, CPlayer::EPlayerUpdateSlot::ePlayerUpdateSlot_Main);

	// Query for the player that owns this extension.
	m_pPlayer = static_cast<CPlayer *>(pGameObject->QueryExtension("Player"));

	// Start with a clean slate and a known state.
	memset(m_cameraModes, 0, sizeof(m_cameraModes));

	// First person camera.
	m_cameraModes [ECameraMode::eCameraMode_FirstPerson] = static_cast<ICameraComponent*>(GetGameObject()->AcquireExtension("FirstPersonCamera"));

	//m_cameraModes [ECameraMode::eCameraMode_FirstPersonHmd] = new CFirstPersonCameraComponent (); // TODO: make a special one for HMD

	// Action RPG Camera.
	m_cameraModes [ECameraMode::eCameraMode_ActionRpg] = static_cast<ICameraComponent*>(GetGameObject()->AcquireExtension("ActionRPGCamera"));

	// Select the initial camera based on their cvar setting for third person.
	if (GetCVars().m_isThirdPerson)
		SetCameraMode(ECameraMode::eCameraMode_ActionRpg, "Initial selection of camera.");
	else
		SetCameraMode(ECameraMode::eCameraMode_FirstPerson, "Initial selection of camera.");
}


void CCameraManagerComponent::Update(SEntityUpdateContext& ctx, int updateSlot)
{
	if (m_cameraModes [m_cameraMode])
	{
		// They might have toggled first / third person mode. If so, we will need to swap cameras now.
		switch (m_cameraMode)
		{
			case ECameraMode::eCameraMode_FirstPerson:
			case ECameraMode::eCameraMode_FirstPersonHmd:
				if (GetCVars().m_isThirdPerson)
					SetCameraMode(ECameraMode::eCameraMode_ActionRpg, "ToggleThirdPerson()");
				break;

			case ECameraMode::eCameraMode_ActionRpg:
				// TODO: Handle HMD displays gracefully.
				if (!GetCVars().m_isThirdPerson)
					SetCameraMode(ECameraMode::eCameraMode_FirstPerson, "ToggleThirdPerson()");
				break;
		}
	}
}


// ***
// *** ICameraManagerComponent
// ***


void CCameraManagerComponent::AttachToEntity(EntityId entityID)
{
	// Let all the cameras know we switch entities. Only a few will care, but it's safer to just let them all handle
	// it. An alternative would be to perform an attach when switching cameras. This is good enough for now. 
	for (unsigned int i = 0; i < ECameraMode::eCameraMode_Last; ++i)
	{
		if (m_cameraModes [i])
			m_cameraModes [i]->AttachToEntity(entityID);
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
			m_cameraModes [m_cameraMode]->OnDeactivate();

		// Tell the new camera it is entering usage.
		if (mode != ECameraMode::eCameraMode_NoCamera)
			m_cameraModes [mode]->OnActivate();

		// Track the previous camera mode, in case we want to switch back to it.
		m_lastCameraMode = m_cameraMode;

		// Set the new mode.
		m_cameraMode = mode;

		// Forced changes to mode here should toggle third person if appropriate.
		switch (m_cameraMode)
		{
			case ECameraMode::eCameraMode_ActionRpg:
				SetCVars().m_isThirdPerson = true;
				break;

			default:
				SetCVars().m_isThirdPerson = false;
				break;
		}

		// Debug.
		CryLogAlways("SetCameraMode from %d to %d, reason - %s", m_lastCameraMode, m_cameraMode, reason);
	}
	else
	{
		// Debug.
		CryLogAlways("SetCameraMode from %d to %d, reason - %s (Skipped)", m_lastCameraMode, m_cameraMode, reason);
	}
}


ICameraComponent* CCameraManagerComponent::GetCamera() const
{
	return m_cameraModes [m_cameraMode];
}


bool CCameraManagerComponent::IsThirdPerson() const
{
	return GetCVars().m_isThirdPerson != 0;
}


void CCameraManagerComponent::ToggleThirdPerson()
{
	SetCVars().m_isThirdPerson = !GetCVars().m_isThirdPerson;
}


// ***
// *** CCameraManagerComponent
// ***


CCameraManagerComponent::CCameraManagerComponent()
{
	// We'll take an initial value for the debug view offset from cvars.
	m_interactiveViewOffset = Vec3FromString(GetCVars().m_debugViewOffset->GetString());
}


CCameraManagerComponent::~CCameraManagerComponent()
{
}


const CCameraManagerComponent::SExternalCVars& CCameraManagerComponent::GetCVars() const
{
	return g_cameraManagerRegistrator;
}


CCameraManagerComponent::SExternalCVars& CCameraManagerComponent::SetCVars()
{
	return g_cameraManagerRegistrator;
}


Vec3 CCameraManagerComponent::GetViewOffset()
{
	return Vec3FromString(GetCVars().m_debugViewOffset->GetString()) + m_interactiveViewOffset; 
}


bool CCameraManagerComponent::OnActionCameraShiftUp(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	if (activationMode && (eAAM_OnPress || eAAM_OnHold))
		m_interactiveViewOffset += Vec3(0.0f, 0.0f, adjustmentAmount);

	return false;
}


bool CCameraManagerComponent::OnActionCameraShiftDown(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	if (activationMode && (eAAM_OnPress || eAAM_OnHold))
		m_interactiveViewOffset += Vec3(0.0f, 0.0f, -adjustmentAmount);

	return false;
}


bool CCameraManagerComponent::OnActionCameraShiftLeft(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	if (activationMode && (eAAM_OnPress || eAAM_OnHold))
		m_interactiveViewOffset += Vec3(-adjustmentAmount, 0.0f, 0.0f);

	return false;
}


bool CCameraManagerComponent::OnActionCameraShiftRight(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	if (activationMode && (eAAM_OnPress || eAAM_OnHold))
		m_interactiveViewOffset += Vec3(adjustmentAmount, 0.0f, 0.0f);

	return false;
}


bool CCameraManagerComponent::OnActionCameraShiftForward(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	if (activationMode && (eAAM_OnPress || eAAM_OnHold))
		m_interactiveViewOffset += Vec3(0.0f, adjustmentAmount, 0.0f);

	return false;
}


bool CCameraManagerComponent::OnActionCameraShiftBackward(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	if (activationMode && (eAAM_OnPress || eAAM_OnHold))
		m_interactiveViewOffset += Vec3(0.0f, -adjustmentAmount, 0.0f);

	return false;
}
