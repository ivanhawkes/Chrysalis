#include <StdAfx.h>

#include "CameraManager.h"
#include "ActionRPGCamera.h"
#include "FirstPersonCamera.h"
#include <Actor/Player/Player.h>
#include <ActionMaps/GameActionMaps.h>


// ***
// *** IGameObjectExtension
// ***


void CCameraManager::GetMemoryUsage(ICrySizer *pSizer) const
{
	pSizer->Add(*this);
	pSizer->Add(m_cameraModes[ECameraMode::FIRST_PERSON]);
	pSizer->Add(m_cameraModes [ECameraMode::ACTION_RPG]);
}


bool CCameraManager::Init(IGameObject * pGameObject)
{
	// Critical this is called.
	SetGameObject(pGameObject);

	// Start with a clean slate and a known state.
	m_lastCameraMode = ECameraMode::NO_CAMERA;
	m_cameraMode = ECameraMode::NO_CAMERA;
	memset(m_cameraModes, 0, sizeof(m_cameraModes));

	// First person camera.
	m_cameraModes [ECameraMode::FIRST_PERSON] = reinterpret_cast<ICamera*>(GetGameObject()->AcquireExtension("FirstPersonCamera"));

	//m_cameraModes [ECameraMode::FIRST_PERSON_HMD] = new CFirstPersonCamera (); // TODO: make a special one for HMD

	// Action RPG Camera.
	m_cameraModes [ECameraMode::ACTION_RPG] = reinterpret_cast<ICamera*>(GetGameObject()->AcquireExtension("ActionRPGCamera"));

	// Once the cameras are created we should be able to select our default camera.
	SetCameraMode(ECameraMode::ACTION_RPG, "Initial selection of camera.");
//	SetCameraMode(ECameraMode::FIRST_PERSON, "Initial selection of camera.");
	
	return true;
}


void CCameraManager::PostInit(IGameObject * pGameObject)
{
	// Allow this instance to be updated every frame.
	pGameObject->EnableUpdateSlot(this, 0);
}


bool CCameraManager::ReloadExtension(IGameObject * pGameObject, const SEntitySpawnParams &params)
{
	ResetGameObject();

	return true;
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


// ***
// *** IGameObjectView
// ***


void CCameraManager::UpdateView(SViewParams& params)
{
	// The last update call should have given us a new updated position and rotation. We now pass those off to the view
	// system. 
	params.SaveLast();
	params.position = m_cameraPose.GetPosition();
	params.rotation = m_cameraPose.GetRotation();

	// TODO: Need to set a FoV or weird stuff happens. Set up a cvar or something.
	// VR devices might need to be queried.
	params.fov = DEG2RAD(60.0f);
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
}