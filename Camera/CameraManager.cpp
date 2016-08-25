#include <StdAfx.h>

#include "CameraManager.h"
#include "ActionRPGCamera.h"
#include "FirstPersonCamera.h"
#include <Actor/Player/Player.h>


// ***
// *** IGameObjectExtension
// ***


void CCameraManager::GetMemoryUsage(ICrySizer *pSizer) const
{
	pSizer->Add(*this);
	pSizer->Add(m_cameraModes[ECameraMode::FIRST_PERSON]);
	pSizer->Add(m_cameraModes [ECameraMode::ACTION_RPG]);
}


void CCameraManager::PostInit(IGameObject * pGameObject)
{
	// Allow this instance to be updated every frame.
	pGameObject->EnableUpdateSlot(this, 0);

	// Start with a clean slate and a known state.
	memset(m_cameraModes, 0, sizeof(m_cameraModes));

	// First person camera.
	m_cameraModes [ECameraMode::FIRST_PERSON] = static_cast<ICamera*>(GetGameObject()->AcquireExtension("FirstPersonCamera"));

	//m_cameraModes [ECameraMode::FIRST_PERSON_HMD] = new CFirstPersonCamera (); // TODO: make a special one for HMD

	// Action RPG Camera.
	m_cameraModes [ECameraMode::ACTION_RPG] = static_cast<ICamera*>(GetGameObject()->AcquireExtension("ActionRPGCamera"));

	// Once the cameras are created we should be able to select our default camera.
	SetCameraMode(ECameraMode::ACTION_RPG, "Initial selection of camera.");
	//	SetCameraMode(ECameraMode::FIRST_PERSON, "Initial selection of camera.");

	// Detect when we become the local player.
	const int requiredEvents [] = { eGFE_BecomeLocalPlayer };
	pGameObject->UnRegisterExtForEvents(this, NULL, 0);
	pGameObject->RegisterExtForEvents(this, requiredEvents, sizeof(requiredEvents) / sizeof(int));

}


void CCameraManager::Update(SEntityUpdateContext& ctx, int updateSlot)
{
	// TODO: How this works has now changed. Missing ability to toggle the view from FP to TP as well.
	// It was on the cameras, but I factored it out of there.

	if (m_cameraModes [m_cameraMode])
	{
		// They might have toggled first / third person mode. If so, we will need to swap cameras now.
		switch (m_cameraMode)
		{
			case ECameraMode::FIRST_PERSON:
			case ECameraMode::FIRST_PERSON_HMD:
				if (CPlayer::GetLocalPlayer()->IsThirdPerson())
				{
					SetCameraMode(ECameraMode::ACTION_RPG, "ToggleThirdPerson()");
				}
				break;

			case ECameraMode::ACTION_RPG:
				if (!CPlayer::GetLocalPlayer()->IsThirdPerson())
				{
					SetCameraMode(ECameraMode::FIRST_PERSON, "ToggleThirdPerson()");
				}
				break;
		}
	}
}


void CCameraManager::HandleEvent(const SGameObjectEvent &event)
{
	// We only register action maps if we are the local player.
	if (event.event == eGFE_BecomeLocalPlayer)
	{
		RegisterActionMaps();
	}
}


// ***
// *** IActionListener
// ***

void CCameraManager::OnAction(const ActionId& action, int activationMode, float value)
{
	// We want to dispatch the action as if it were for the character that this player is currently attached onto.
	m_actionHandler.Dispatch(this, GetEntityId(), action, activationMode, value);
}


// ***
// *** ICameraManager
// ***


void CCameraManager::AttachToEntity(EntityId entityID)
{
	// Let all the cameras know we switch entities. Only a few will care, but it's safer to just let them all handle
	// it. An alternative would be to perform an attach when switching cameras. This is good enough for now. 
	for (unsigned int i = 0; i < ECameraMode::_LAST; ++i)
	{
		if (m_cameraModes [i])
			m_cameraModes [i]->AttachToEntity(entityID);
	}
}


ECameraMode CCameraManager::GetCameraMode()
{
	return m_cameraMode;
}


void CCameraManager::SetCameraMode(ECameraMode mode, const char* reason)
{
	if (m_cameraMode != mode)
	{
		// Notify the cameras that we're changing.
		if (m_cameraMode != ECameraMode::NO_CAMERA)
			m_cameraModes [m_cameraMode]->OnDeactivate();
		if (mode != ECameraMode::NO_CAMERA)
			m_cameraModes [mode]->OnActivate();

		m_lastCameraMode = m_cameraMode;
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


ICamera* CCameraManager::GetCamera() const
{
	return m_cameraModes [m_cameraMode];
}


// ***
// *** CCameraManager
// ***


CCameraManager::CCameraManager()
{}


CCameraManager::~CCameraManager()
{
	// Clean up the cameras.
	GetGameObject()->ReleaseExtension("FirstPersonCamera");
	GetGameObject()->ReleaseExtension("ActionRPGCamera");

	// Clean up our action map usage.
	IActionMapManager* pActionMapManager = gEnv->pGame->GetIGameFramework()->GetIActionMapManager();
	pActionMapManager->EnableActionMap("camera", false);
	pActionMapManager->RemoveExtraActionListener(this, "camera");
	GetGameObject()->ReleaseActions(this);
}


void CCameraManager::RegisterActionMaps()
{
	// Populate the action handler callbacks so that we get action map events.
	InitializeActionHandler();

	// Try and enable the "camera" actions.
	IActionMapManager* pActionMapManager = gEnv->pGame->GetIGameFramework()->GetIActionMapManager();
	pActionMapManager->EnableActionMap("camera", true);
	pActionMapManager->AddExtraActionListener(this, "camera");
	GetGameObject()->CaptureActions(this);
}


void CCameraManager::InitializeActionHandler()
{
	// Camera movements.
	m_actionHandler.AddHandler(ActionId("camera_shift_up"), &CCameraManager::OnActionCameraShiftUp);
	m_actionHandler.AddHandler(ActionId("camera_shift_down"), &CCameraManager::OnActionCameraShiftDown);
	m_actionHandler.AddHandler(ActionId("camera_shift_left"), &CCameraManager::OnActionCameraShiftLeft);
	m_actionHandler.AddHandler(ActionId("camera_shift_right"), &CCameraManager::OnActionCameraShiftRight);
	m_actionHandler.AddHandler(ActionId("camera_shift_forward"), &CCameraManager::OnActionCameraShiftForward);
	m_actionHandler.AddHandler(ActionId("camera_shift_backward"), &CCameraManager::OnActionCameraShiftBackward);
}


bool CCameraManager::OnActionCameraShiftUp(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	if (activationMode && (eAAM_OnPress || eAAM_OnHold))
		m_viewOffset += Vec3(0.0f, 0.0f, adjustmentAmount);

	return false;
}


bool CCameraManager::OnActionCameraShiftDown(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	if (activationMode && (eAAM_OnPress || eAAM_OnHold))
		m_viewOffset += Vec3(0.0f, 0.0f, -adjustmentAmount);

	return false;
}


bool CCameraManager::OnActionCameraShiftLeft(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	if (activationMode && (eAAM_OnPress || eAAM_OnHold))
		m_viewOffset += Vec3(-adjustmentAmount, 0.0f, 0.0f);

	return false;
}


bool CCameraManager::OnActionCameraShiftRight(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	if (activationMode && (eAAM_OnPress || eAAM_OnHold))
		m_viewOffset += Vec3(adjustmentAmount, 0.0f, 0.0f);

	return false;
}


bool CCameraManager::OnActionCameraShiftForward(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	if (activationMode && (eAAM_OnPress || eAAM_OnHold))
		m_viewOffset += Vec3(0.0f, adjustmentAmount, 0.0f);

	return false;
}


bool CCameraManager::OnActionCameraShiftBackward(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	if (activationMode && (eAAM_OnPress || eAAM_OnHold))
		m_viewOffset += Vec3(0.0f, -adjustmentAmount, 0.0f);

	return false;
}
