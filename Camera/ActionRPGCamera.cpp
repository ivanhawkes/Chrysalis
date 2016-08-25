#include <StdAfx.h>

#include "ActionRPGCamera.h"
#include <Game/Game.h>
#include <IActorSystem.h>
#include <Actor/Player/Player.h>
#include <Actor/Player/PlayerInput/IPlayerInput.h>


// ***
// *** IGameObjectExtension
// ***


void CActionRPGCamera::PostInit(IGameObject * pGameObject)
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


bool CActionRPGCamera::ReloadExtension(IGameObject * pGameObject, const SEntitySpawnParams &params)
{
	ResetGameObject();

	// If we're meant to be active, capture the view.
	OnActivate();

	return true;
}


void CActionRPGCamera::Release()
{
	// Release the view.
	GetGameObject()->ReleaseView(this);
	gEnv->pGame->GetIGameFramework()->GetIViewSystem()->RemoveView(m_pView);
	m_pView = nullptr;

	delete this;
}


void CActionRPGCamera::Update(SEntityUpdateContext& ctx, int updateSlot)
{
	// Default on failure is to return a cleanly constructed blank camera pose.
	CCameraPose newCameraPose { CCameraPose() };

	auto pEntity = gEnv->pEntitySystem->GetEntity(m_targetEntityID);
	auto pPlayerInput = CPlayer::GetLocalPlayer()->GetPlayerInput();

	// Calculate the new zoom goal after asking the player input for zoom level deltas.
	m_zoomGoal = clamp_tpl(m_lastZoomGoal + pPlayerInput->GetZoomDelta() * m_zoomStep, m_zoomMin, m_zoomMax);
	m_lastZoomGoal = m_zoomGoal;

	// Apply the player input rotation for this frame, and limit the pitch / yaw movement according to the set max and min values.
	m_viewPitch += pPlayerInput->GetPitchDelta();
	m_viewPitch = clamp_tpl(m_viewPitch, m_pitchMin, m_pitchMax);
	m_viewYaw += pPlayerInput->GetYawDelta();
	m_viewYaw = clamp_tpl(m_viewYaw, m_yawMin, m_yawMax);

	// Yaw should wrap around if it exceeds it's values. This is a bit simplistic, but will work most of the time.
	if (m_viewYaw > g_PI)
		m_viewYaw -= g_PI2;
	if (m_viewYaw < -g_PI)
		m_viewYaw += g_PI2;

	// Skip the PostUpdate if we don't need to process the movement. Whenever we skip over frames we have to also skip
	// running interpolation for the following frame. 
	if ((pEntity) && (!gEnv->IsCutscenePlaying()))
	{
		// Interpolate towards the desired zoom position.
		Interpolate(m_zoom, m_zoomGoal, m_zoomSpeed, ctx.fFrameTime);

		// Get the entity we are targeting.
		auto pTargetEntity = gEnv->pEntitySystem->GetEntity(m_targetEntityID);
		if (pTargetEntity)
		{
			// Calculate pitch and yaw movements to apply both prior to and after positioning the camera.
			Quat quatPreTransYP = Quat(Ang3(m_viewPitch, 0.0f, m_viewYaw));
			Quat quatPostTransYP = Quat::CreateRotationXYZ(Ang3(m_viewPitch * m_reversePitchTilt, 0.0f, 0.0f));

			// Target and aim position come from the entity position.
			Vec3 vecTargetAimPosition = GetTargetAimPosition(pTargetEntity);

			// The distance from the view to the target.
			float shapedZoom = (m_zoom * m_zoom) / (m_zoomMax * m_zoomMax);
			float zoomDistance = m_targetDistance * shapedZoom;

			// Calculate the target rotation and slerp it if we can.
			Quat quatTargetRotationGoal = m_quatTargetRotation * quatPreTransYP;
			Quat quatTargetRotation;
			if (m_skipInterpolation)
			{
				m_quatLastTargetRotation = quatTargetRotation = quatTargetRotationGoal;
			}
			else
			{
				quatTargetRotation = Quat::CreateSlerp(m_quatLastTargetRotation, quatTargetRotationGoal, ctx.fFrameTime * m_slerpSpeed);
				m_quatLastTargetRotation = quatTargetRotation;
			}

			// Work out where to place the new initial position. We will be using a unit vector facing forward Y
			// as the starting place and applying rotations from the target bone and player camera movements.
			Vec3 vecViewPosition = vecTargetAimPosition + (quatTargetRotation * (Vec3(0.0f, 1.0f, 0.0f) * zoomDistance))
				+ quatTargetRotation * m_viewPositionOffset;

			// By default, we try and aim the camera at the target, taking into account the current mouse yaw and pitch values.
			// Since the target and aim can actually be the same we need to use a safe version of the normalised quaternion to
			// prevent errors.
			Quat quatViewRotationGoal = Quat::CreateRotationVDir((vecTargetAimPosition - vecViewPosition).GetNormalizedSafe());

			// Use a slerp to smooth out fast camera rotations.
			Quat quatViewRotation;
			if (m_skipInterpolation)
			{
				m_quatLastViewRotation = quatViewRotation = quatViewRotationGoal;
				m_skipInterpolation = false;
			}
			else
			{
				quatViewRotation = Quat::CreateSlerp(m_quatLastViewRotation, quatViewRotationGoal, ctx.fFrameTime * m_slerpSpeed);
				m_quatLastViewRotation = quatViewRotation;
			}

			// Apply a final translation to both the view position and the aim position.
			vecViewPosition += quatViewRotation * m_aimPositionOffset;

			// Gimbal style rotation after it's moved into it's initial position.
			Quat quatOrbitRotation = quatViewRotation * quatPostTransYP;

			// Perform a collision detection. Note, any collisions will disallow use of interpolated
			// camera movement.
			// NOTE: This is broken in two ways. Collisions always hit the actor model, and I refactored
			// vecTargetAimPosition from two variables which held the same value - so the line beneath
			// is bupkis / wrong.
			//Vec3 vecTargetAimPosition = vecTargetAimPosition + quatViewRotation * m_aimPositionOffset;
			CollisionDetection (vecTargetAimPosition, vecViewPosition);

#if defined(_DEBUG)
			//			if (m_bDebug)
			{
				// Initial position for our target.
				//gEnv->pRenderer->GetIRenderAuxGeom()->DrawSphere(vecTargetAimPosition, 0.16f, ColorB(0, 0, 128));
				//gEnv->pRenderer->GetIRenderAuxGeom()->DrawSphere(vecTargetAimPosition + quatTargetRotation * m_viewPositionOffset, 0.06f, ColorB(0, 128, 0));

				//// Initial aim position.
				//gEnv->pRenderer->GetIRenderAuxGeom()->DrawSphere(vecTargetAimPosition, 0.08f, ColorB(128, 0, 0));
			}
#endif

			// Track our last position.
			m_vecLastPosition = vecViewPosition;

			// Return the newly calculated pose.
			newCameraPose = CCameraPose(vecViewPosition, quatOrbitRotation);
			SetCameraPose(newCameraPose);

			return;
		}
	}

	// We set the pose, regardless of the result.
	SetCameraPose(newCameraPose);

	// If we made it here, we will want to avoid interpolating, since the last frame result will be wrong in some way.
	m_skipInterpolation = true;
}


// ***
// *** IGameObjectView
// ***


void CActionRPGCamera::UpdateView(SViewParams& params)
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
// *** ICamera
// ***


void CActionRPGCamera::AttachToEntity(EntityId entityId)
{
	// Store the target entity.
	m_targetEntityID = entityId;

	
	// No interpolation, since the camera needs to jump into position.
	m_skipInterpolation = true;

	// Resolve the entity ID if possible.
	auto pTargetEntity = gEnv->pEntitySystem->GetEntity(m_targetEntityID);
	if (pTargetEntity)
	{
		// Give the camera an initial orientation based on the entity's rotation.
		m_quatTargetRotation = Quat::CreateRotationVDir(pTargetEntity->GetForwardDir());
	}
	else
	{
		// Use a sensible default value.
		m_quatTargetRotation = Quat(IDENTITY);
	}

	// Reset this.
	m_quatLastTargetRotation = m_quatTargetRotation;
}


void CActionRPGCamera::OnActivate()
{
	gEnv->pGame->GetIGameFramework()->GetIViewSystem()->SetActiveView(m_pView);
	GetGameObject()->CaptureView(this);
}


void CActionRPGCamera::OnDeactivate()
{
	GetGameObject()->ReleaseView(this);
}


// ***
// *** CActionRPGCamera
// ***


CActionRPGCamera::CActionRPGCamera()
{
	ResetCamera();
}


CActionRPGCamera::~CActionRPGCamera()
{
}


void CActionRPGCamera::ResetCamera()
{
	// Zoom can default to it's mid-point.
	m_lastZoomGoal = m_zoomGoal = m_zoom = (m_zoomMax + m_zoomMin) / 2;

	// Pitch and yaw can default to their mid-points.
	m_viewPitch = (m_pitchMax + m_pitchMin) / 2;
	//m_viewYaw = (m_yawMax + m_yawMin) / 2;
	
	// HACK: I'd rather the default be from behind the character instead.
	m_viewYaw = m_yawMax;
}


// ***
// *** Camera movement and updates.
// ***


Vec3 CActionRPGCamera::GetTargetAimPosition(IEntity* const pEntity)
{
	Vec3 position { AverageEyePosition };

	if (pEntity)
	{
		// If we are attached to an entity that is an actor we can use their eye position.
		Vec3 localEyePosition = position;
		auto pActor = gEnv->pGame->GetIGameFramework()->GetIActorSystem()->GetActor(m_targetEntityID);
		if (pActor)
			localEyePosition = pActor->GetLocalEyePos();

		// HACK: TESTING - moving eye position back to see the actor during testing.
		Vec3 debugEyeOffset = Vec3(ZERO);
		//Vec3 debugEyeOffset = pEntity->GetWorldRotation() * (Vec3(-FORWARD_DIRECTION) * 0.6f);

		// Pose is based on entity position and the eye position.
		// We will use the rotation of the entity as a base, and apply pitch based on our own reckoning.
		position = pEntity->GetPos() + localEyePosition + debugEyeOffset;
		
		// DEBUG
		//gEnv->pRenderer->GetIRenderAuxGeom()->DrawSphere(position, 0.05f, ColorB(0, 0, 255, 255));
	}

	return position;
}


bool CActionRPGCamera::CollisionDetection(const Vec3& Goal, Vec3& CameraPosition)
{
	bool updatedCameraPosition = false;

	// Try and update where the player is aiming.
	// TODO: need to skip the player's geometry and if they are in a vehicle, that needs skipping too.
	ray_hit rayhit;
	static IPhysicalEntity* pSkipEnts [10];

	// Skip the target actor for this.
	pSkipEnts [0] = gEnv->pEntitySystem->GetEntity(m_targetEntityID)->GetPhysics();

	// Perform the ray cast.
	int hits = gEnv->pPhysicalWorld->RayWorldIntersection(Goal,
		CameraPosition - Goal,
		ent_static | ent_sleeping_rigid | ent_rigid | ent_independent | ent_terrain,
		rwi_stop_at_pierceable | rwi_colltype_any,
		&rayhit,
		1, pSkipEnts, 2);

	if (hits)
	{
		CameraPosition = rayhit.pt;
		updatedCameraPosition = true;
	}

	return updatedCameraPosition;
}
