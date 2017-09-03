#include <StdAfx.h>

#include "ActionRPGCameraComponent.h"
#include "Components/Player/PlayerComponent.h"
#include <Components/Player/Input/IPlayerInputComponent.h>
#include <Utility/StringConversions.h>
#include <Console/CVars.h>
#include <Actor/Actor.h>


namespace Chrysalis
{
void CActionRPGCameraComponent::Register(Schematyc::CEnvRegistrationScope& componentScope)
{
}


void CActionRPGCameraComponent::ReflectType(Schematyc::CTypeDesc<CActionRPGCameraComponent>& desc)
{
	desc.SetGUID(CActionRPGCameraComponent::IID());
	desc.SetEditorCategory("Cameras");
	desc.SetLabel("Action RPG Camera");
	desc.SetDescription("An action RPG style of camera that orbits around a target.");
	desc.SetIcon("icons:ObjectTypes/light.ico");
	desc.SetComponentFlags({ IEntityComponent::EFlags::Transform, IEntityComponent::EFlags::Socket, IEntityComponent::EFlags::Attach, IEntityComponent::EFlags::ClientOnly });
}


// #TODO: ILH I've dumped this here to get the code to compile again, but it really needs to be switched to whatever
// the new function is. This was grabbed from the GameSDK in GameUtils.h
ILINE void Interpolate(float& actual, float goal, float speed, float frameTime, float limit = 0)
{
	float delta(goal - actual);

	if (limit > 0.001f)
	{
		delta = max(min(delta, limit), -limit);
	}

	actual += delta * min(frameTime * speed, 1.0f);
}


void CActionRPGCameraComponent::Initialize()
{
	// It's a good idea to use the entity as a default for our target entity.
	m_targetEntityID = GetEntityId();

	// We are usually hosted in the same entity as a camera manager. Use it if you can find one.
	m_pCameraManager = GetEntity()->GetComponent<CCameraManagerComponent>();

	m_skipInterpolation = true;
}


void CActionRPGCameraComponent::ProcessEvent(SEntityEvent& event)
{
	switch (event.event)
	{
		case ENTITY_EVENT_UPDATE:
			Update();
			UpdateView();
			break;
	}
}


void CActionRPGCameraComponent::OnShutDown()
{
}


void CActionRPGCameraComponent::Update()
{
	auto pEntity = gEnv->pEntitySystem->GetEntity(m_targetEntityID);
	auto pPlayer = CPlayerComponent::GetLocalPlayer();
	auto pPlayerInput = pPlayer->GetPlayerInput();

	if (pPlayerInput)
	{
		// If the player zoomed all the way in, switch to the first person camera.
		float tempZoomGoal = m_lastZoomGoal + pPlayerInput->GetZoomDelta() * g_cvars.m_actionRPGCameraZoomStep;
		if (m_pCameraManager)
		{
			if ((tempZoomGoal < g_cvars.m_actionRPGCameraZoomMin) && (m_pCameraManager->IsThirdPerson()))
				m_pCameraManager->ToggleThirdPerson();
		}

		// Calculate the new zoom goal after asking the player input for zoom level deltas.
		m_zoomGoal = clamp_tpl(tempZoomGoal, g_cvars.m_actionRPGCameraZoomMin, g_cvars.m_actionRPGCameraZoomMax);
		m_lastZoomGoal = m_zoomGoal;

		// Apply the player input rotation for this frame, and limit the pitch / yaw movement according to the set max and min values.
		if (pPlayer->IsCameraMovementAllowed())
		{
			m_viewPitch += pPlayerInput->GetMousePitchDelta() + pPlayerInput->GetXiPitchDelta();
			m_viewPitch = clamp_tpl(m_viewPitch, DEG2RAD(g_cvars.m_actionRPGCameraPitchMin), DEG2RAD(g_cvars.m_actionRPGCameraPitchMax));
			m_viewYaw += pPlayerInput->GetMouseYawDelta() - pPlayerInput->GetXiYawDelta();
		}

		// Yaw should wrap around if it exceeds it's values. This is a bit simplistic, but will work most of the time.
		if (m_viewYaw > gf_PI)
			m_viewYaw -= gf_PI2;
		if (m_viewYaw < -gf_PI)
			m_viewYaw += gf_PI2;

		// Skip the update if we don't need to process the movement. Whenever we skip over frames we have to also skip
		// running interpolation for the following frame. 
		if ((pEntity) && (!gEnv->IsCutscenePlaying()))
		{
			// Interpolate towards the desired zoom position.
			Interpolate(m_zoom, m_zoomGoal, g_cvars.m_actionRPGCameraZoomSpeed, gEnv->pTimer->GetFrameTime());

			// Get the entity we are targeting.
			auto pTargetEntity = gEnv->pEntitySystem->GetEntity(m_targetEntityID);
			if (pTargetEntity)
			{
				// Calculate pitch and yaw movements to apply both prior to and after positioning the camera.
				Quat quatPreTransYP = Quat(Ang3(m_viewPitch, 0.0f, m_viewYaw));
				Quat quatPostTransYP = Quat::CreateRotationXYZ(Ang3(m_viewPitch * g_cvars.m_actionRPGCameraReversePitchTilt, 0.0f, 0.0f));

				// Target and aim position come from the entity position.
				Vec3 vecTargetAimPosition = GetTargetAimPosition(pTargetEntity);

				// The distance from the view to the target.
				float shapedZoom = (m_zoom * m_zoom) / (g_cvars.m_actionRPGCameraZoomMax * g_cvars.m_actionRPGCameraZoomMax);
				float zoomDistance = g_cvars.m_actionRPGCameraTargetDistance * shapedZoom;

				// Calculate the target rotation and SLERP it if we can.
				Quat quatTargetRotationGoal = m_quatTargetRotation * quatPreTransYP;
				Quat quatTargetRotation;
				if (m_skipInterpolation)
				{
					m_quatLastTargetRotation = quatTargetRotation = quatTargetRotationGoal;
				}
				else
				{
					quatTargetRotation = Quat::CreateSlerp(m_quatLastTargetRotation, quatTargetRotationGoal, gEnv->pTimer->GetFrameTime() * g_cvars.m_actionRPGCameraSlerpSpeed);
					m_quatLastTargetRotation = quatTargetRotation;
				}

				// Work out where to place the new initial position. We will be using a unit vector facing forward Y
				// as the starting place and applying rotations from the target bone and player camera movements.
				Vec3 viewPositionOffset = Vec3FromString(g_cvars.m_actionRPGCameraViewPositionOffset->GetString());
				Vec3 vecViewPosition = vecTargetAimPosition + (quatTargetRotation * (FORWARD_DIRECTION * zoomDistance)) + quatTargetRotation * viewPositionOffset;

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
					quatViewRotation = Quat::CreateSlerp(m_quatLastViewRotation, quatViewRotationGoal, gEnv->pTimer->GetFrameTime() * g_cvars.m_actionRPGCameraSlerpSpeed);
					m_quatLastViewRotation = quatViewRotation;
				}

				// Apply a final translation to both the view position and the aim position.
				Vec3 aimPositionOffset = Vec3FromString(g_cvars.m_actionRPGCameraAimPositionOffset->GetString());
				vecViewPosition += quatViewRotation * aimPositionOffset;

				// Gimbal style rotation after it's moved into it's initial position.
				Quat quatOrbitRotation = quatViewRotation * quatPostTransYP;

				// Perform a collision detection. Note, any collisions will disallow use of interpolated camera movement.
				CollisionDetection(vecTargetAimPosition, vecViewPosition);

#if defined(_DEBUG)
				if (g_cvars.m_actionRPGCameraDebug)
				{
					gEnv->pRenderer->GetIRenderAuxGeom()->DrawSphere(vecTargetAimPosition, 0.04f, ColorB(128, 0, 0));
					gEnv->pRenderer->GetIRenderAuxGeom()->DrawSphere(vecTargetAimPosition + quatTargetRotation * viewPositionOffset, 0.04f, ColorB(0, 128, 0));
				}
#endif

				// Track our last position.
				m_vecLastPosition = vecViewPosition;

				// Return the newly calculated pose.
				m_cameraMatrix = Matrix34::Create(Vec3(1.0f), quatOrbitRotation, vecViewPosition);

				return;
			}
		}
	}

	// The calulation failed somewhere, so just use an default matrix.
	m_cameraMatrix = Matrix34::Create(Vec3(1.0f), IDENTITY, ZERO);

	// If we made it here, we will want to avoid interpolating, since the last frame result will be wrong in some way.
	m_skipInterpolation = true;
}


// ***
// *** ICameraComponent
// ***


void CActionRPGCameraComponent::AttachToEntity(EntityId entityId)
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


void CActionRPGCameraComponent::OnActivate()
{
	m_EventMask |= BIT64(ENTITY_EVENT_UPDATE);
	GetEntity()->UpdateComponentEventMask(this);
	m_skipInterpolation = true;
	ResetCamera();
}


void CActionRPGCameraComponent::OnDeactivate()
{
	m_EventMask &= ~BIT64(ENTITY_EVENT_UPDATE);
	GetEntity()->UpdateComponentEventMask(this);
}


// ***
// *** CActionRPGCameraComponent
// ***


void CActionRPGCameraComponent::ResetCamera()
{
	// Zoom can default to it's mid-point.
	m_lastZoomGoal = m_zoomGoal = m_zoom = (g_cvars.m_actionRPGCameraZoomMax + g_cvars.m_actionRPGCameraZoomMin) / 2;

	// Pitch and yaw can default to their mid-points.
	m_viewPitch = (DEG2RAD(g_cvars.m_actionRPGCameraPitchMax) + DEG2RAD(g_cvars.m_actionRPGCameraPitchMin)) / 2;

	// We'll start from behind our character as a default.
	if (auto pTargetEntity = gEnv->pEntitySystem->GetEntity(m_targetEntityID))
	{
		// The camera should default to being behind the actor.
		Vec3 backward = pTargetEntity->GetForwardDir();
		backward.Flip();
		m_viewYaw = CCamera::CreateAnglesYPR(backward).x;
	}
	else
	{
		m_viewYaw = 180.0f;
	}
}


Vec3 CActionRPGCameraComponent::GetTargetAimPosition(IEntity* const pEntity)
{
	Vec3 position { AverageEyePosition };

	if (pEntity)
	{
		// If we are attached to an entity that is an actor we can use their eye position.
		Vec3 localEyePosition = position;
		auto pActor = CActorComponent::GetActor(m_targetEntityID);
		if (pActor)
			localEyePosition = pActor->GetLocalEyePos();

		// Pose is based on entity position and the eye position.
		// We will use the rotation of the entity as a base, and apply pitch based on our own reckoning.
		position = pEntity->GetPos() + localEyePosition;

#if defined(_DEBUG)
		if (g_cvars.m_actionRPGCameraDebug)
		{
			gEnv->pRenderer->GetIRenderAuxGeom()->DrawSphere(position, 0.04f, ColorB(0, 0, 255, 255));
		}
#endif
	}

	return position;
}


bool CActionRPGCameraComponent::CollisionDetection(const Vec3& Goal, Vec3& CameraPosition)
{
	bool updatedCameraPosition = false;

	// Skip the target actor for this.
	ray_hit rayhit;
	static IPhysicalEntity* pSkipEnts [10];
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
}