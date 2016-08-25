#include <StdAfx.h>

#include "FirstPersonCamera.h"
#include <Game/Game.h>
#include <IActorSystem.h>
#include <Actor/Player/Player.h>
#include <Actor/Player/PlayerInput/IPlayerInput.h>


// ***
// *** IGameObjectExtension
// ***


void CFirstPersonCamera::PostInit(IGameObject * pGameObject)
{
	// Allow this instance to be updated every frame.
	pGameObject->EnableUpdateSlot(this, 0);

	// It's a good idea to use the entity as a default for our target entity.
	m_targetEntityID = GetEntityId();

	// Create a new view and link it to this entity.
	auto pViewSystem = gEnv->pGame->GetIGameFramework()->GetIViewSystem();
	m_pView = pViewSystem->CreateView();
	m_pView->LinkTo(GetGameObject());

	// We are usually hosted in the same entity as a camera manager. Use it if you can find one.
	m_pCameraManager = static_cast<ICameraManager*> (pGameObject->QueryExtension("CameraManager"));
}


bool CFirstPersonCamera::ReloadExtension(IGameObject * pGameObject, const SEntitySpawnParams &params)
{
	ResetGameObject();

	// If we're meant to be active, capture the view.
	OnActivate();

	return true;
}


void CFirstPersonCamera::Release()
{
	// Release the view.
	GetGameObject()->ReleaseView(this);
	gEnv->pGame->GetIGameFramework()->GetIViewSystem()->RemoveView(m_pView);
	m_pView = nullptr;

	delete this;
}


void CFirstPersonCamera::Update(SEntityUpdateContext& ctx, int updateSlot)
{
	// Default on failure is to return a cleanly constructed blank camera pose.
	CCameraPose newCameraPose { CCameraPose() };

	// Resolve the entity.
	auto pEntity = gEnv->pEntitySystem->GetEntity(m_targetEntityID);
	if (pEntity)
	{
		// It's possible there is no actor to query for eye position, in that case, return a safe default
		// value for an average height person.
		Vec3 localEyePosition { AverageEyePosition };

		// If we are attached to an entity that is an actor we can use their eye position.
		auto pActor = gEnv->pGame->GetIGameFramework()->GetIActorSystem()->GetActor(m_targetEntityID);
		if (pActor)
			localEyePosition = pActor->GetLocalEyePos();

		// HACK: TESTING - moving eye position back to see the actor during testing.
		Vec3 debugEyeOffset = Vec3(ZERO);
		//Vec3 debugEyeOffset = pEntity->GetWorldRotation() * (Vec3(-FORWARD_DIRECTION) * 0.6f);

		// Apply the player input rotation for this frame, and limit the pitch / yaw movement according to the set max and min values.
		auto pPlayerInput = CPlayer::GetLocalPlayer()->GetPlayerInput();
		m_viewPitch -= pPlayerInput->GetPitchDelta();
		m_viewPitch = clamp_tpl(m_viewPitch, m_pitchMin, m_pitchMax);

		// Pose is based on entity position and the eye position.
		// We will use the rotation of the entity as a base, and apply pitch based on our own reckoning.
		const Vec3 position = pEntity->GetPos() + localEyePosition;
		const Quat rotation = pEntity->GetRotation() * Quat (Ang3(m_viewPitch, 0.0f, 0.0f));
		newCameraPose = CCameraPose(position + debugEyeOffset, rotation);

		// DEBUG
		//gEnv->pRenderer->GetIRenderAuxGeom()->DrawSphere(position, 0.05f, ColorB(0, 0, 255, 255));
	}

	// We set the pose, regardless of the result.
	SetCameraPose(newCameraPose);
}


// ***
// *** ICamera
// ***


void CFirstPersonCamera::OnActivate()
{
	gEnv->pGame->GetIGameFramework()->GetIViewSystem()->SetActiveView(m_pView);
	GetGameObject()->CaptureView(this);
}


void CFirstPersonCamera::OnDeactivate()
{
	GetGameObject()->ReleaseView(this);
}


// ***
// *** IGameObjectView
// ***


void CFirstPersonCamera::UpdateView(SViewParams& params)
{
	// The last update call should have given us a new updated position and rotation.
	// We now pass those off to the view system. 
	params.SaveLast();
	params.position = GetCameraPose().GetPosition();
	params.rotation = GetCameraPose().GetRotation();

	if (m_pCameraManager)
		params.position += GetCameraPose().GetRotation() * m_pCameraManager->GetViewOffset();

	// TODO: Need to set a FoV or weird stuff happens. Set up a cvar or something.
	// VR devices might need to be queried.
	params.fov = DEG2RAD(60.0f);
}


// ***
// *** CFirstPersonCamera
// ***


CFirstPersonCamera::CFirstPersonCamera()
{
	ResetCamera();
}


CFirstPersonCamera::~CFirstPersonCamera()
{
}


void CFirstPersonCamera::ResetCamera()
{
	// Pitch can default to the mid-point.
	m_viewPitch = (m_pitchMax + m_pitchMin) / 2;
}


void CFirstPersonCamera::AttachToEntity(EntityId entityId)
{
	// Store the target entity.
	m_targetEntityID = entityId;
}
