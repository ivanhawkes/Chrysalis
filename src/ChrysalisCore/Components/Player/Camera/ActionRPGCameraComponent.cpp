#include <StdAfx.h>

#include "ActionRPGCameraComponent.h"
#include <CryCore/StaticInstanceList.h>
#include "CrySchematyc/Env/Elements/EnvComponent.h"
#include "CrySchematyc/Env/IEnvRegistrar.h"
#include <CryRenderer/IRenderAuxGeom.h>
#include "Components/Player/PlayerComponent.h"
#include <Components/Player/Input/PlayerInputComponent.h>
#include <Utility/StringConversions.h>
#include <Console/CVars.h>
#include <Components/Actor/ActorComponent.h>
#include <CryGame/GameUtils.h>


namespace Chrysalis
{
static void RegisterActionRPGCameraComponent(Schematyc::IEnvRegistrar& registrar)
{
	Schematyc::CEnvRegistrationScope scope = registrar.Scope(IEntity::GetEntityScopeGUID());
	{
		Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(CActionRPGCameraComponent));
		// Functions
		{
		}
	}
}


void CActionRPGCameraComponent::ReflectType(Schematyc::CTypeDesc<CActionRPGCameraComponent>& desc)
{
	desc.SetGUID(CActionRPGCameraComponent::IID());
	desc.SetEditorCategory("Cameras");
	desc.SetLabel("Action RPG Camera");
	desc.SetDescription("An action RPG style of camera that orbits around a target.");
	desc.SetIcon("icons:ObjectTypes/light.ico");
	desc.SetComponentFlags({IEntityComponent::EFlags::Socket, IEntityComponent::EFlags::Attach, IEntityComponent::EFlags::ClientOnly});
}


void CActionRPGCameraComponent::Initialize()
{
	// It's a good idea to use the entity as a default for our target entity.
	m_targetEntityID = GetEntityId();

	// We are usually hosted in the same entity as a camera manager. Use it if you can find one.
	m_pCameraManager = GetEntity()->GetComponent<CCameraManagerComponent>();

	m_skipInterpolation = true;
}


void CActionRPGCameraComponent::ProcessEvent(const SEntityEvent& event)
{
	switch (event.event)
	{
		case EEntityEvent::Update:
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
	UpdateZoom();

	// Do whichever update is appropriate for the current view.
	if (IsViewFirstPerson())
		UpdateFirstPerson();
	else
		UpdateThirdPerson();
}


void CActionRPGCameraComponent::UpdateZoom()
{
	// If the player zoomed all the way in, switch to the first person camera.
	float tempZoomGoal = m_lastZoomGoal + m_pCameraManager->GetZoomDelta() * g_cvars.m_actionRPGCameraZoomStep;

	// Calculate the new zoom goal after asking the player input for zoom level deltas.
	m_zoomGoal = clamp_tpl(tempZoomGoal, g_cvars.m_actionRPGCameraZoomMin, g_cvars.m_actionRPGCameraZoomMax);
	m_lastZoomGoal = m_zoomGoal;

	// Should the camera view change to first person? We're using the goal instead of the final result because stepping
	// to the m_zoom may return a result that is fractionally higher than the actual minimum zoom, thus never switching
	// to the first person view. 
	if (m_zoomGoal <= g_cvars.m_actionRPGCameraZoomMin)
	{
		if (!IsViewFirstPerson())
			ToggleFirstPerson();
	}
	else
	{
		if (IsViewFirstPerson())
			ToggleFirstPerson();
	}

	// Interpolate towards the desired zoom position.
	Interpolate(m_zoom, m_zoomGoal, g_cvars.m_actionRPGCameraZoomSpeed, gEnv->pTimer->GetFrameTime());
}


void CActionRPGCameraComponent::UpdateFirstPerson()
{
	auto pPlayer = CPlayerComponent::GetLocalPlayer();
	auto pPlayerInput = pPlayer->GetPlayerInput();

	// Default on failure is to return a cleanly constructed default matrix.
	Matrix34 newCameraMatrix = Matrix34::Create(Vec3(1.0f), IDENTITY, ZERO);

	if (pPlayerInput)
	{
		// Resolve the entity.
		if (auto pEntity = gEnv->pEntitySystem->GetEntity(m_targetEntityID))
		{
			// It's possible there is no actor to query for eye position, in that case, return a safe default
			// value for an average height person.
			Vec3 localEyePosition {AverageEyePosition};

			// If we are attached to an entity that is an actor we can use their eye position.
			auto pActor = CActorComponent::GetActor(m_targetEntityID);
			if (pActor)
				localEyePosition = pActor->GetLocalEyePos();

			// Apply the player input rotation for this frame, and limit the pitch / yaw movement according to the set max and min values.
			if (pPlayer->GetinteractionState().IsCameraMovementAllowed())
			{
				m_viewPitch -= pPlayerInput->GetMousePitchDelta() - pPlayerInput->GetXiPitchDelta();
				m_viewPitch = clamp_tpl(m_viewPitch, DEG2RAD(g_cvars.m_firstPersonCameraPitchMin), DEG2RAD(g_cvars.m_firstPersonCameraPitchMax));
			}

			// Pose is based on entity position and the eye position.
			// We will use the rotation of the entity as a base, and apply pitch based on our own reckoning.
			const Vec3 position = pEntity->GetPos() + localEyePosition;
			const Quat rotation = pEntity->GetRotation() * Quat(Ang3(m_viewPitch, 0.0f, 0.0f));
			newCameraMatrix = Matrix34::Create(Vec3(1.0f), rotation, position + rotation * m_pCameraManager->GetViewOffset());

#if defined(_DEBUG)
			if (g_cvars.m_firstPersonCameraDebug)
			{
				gEnv->pRenderer->GetIRenderAuxGeom()->DrawSphere(position, 0.04f, ColorB(0, 0, 255, 255));
			}
#endif
		}
	}

	// Store the new matrix for later.
	m_cameraMatrix = newCameraMatrix;
}


void CActionRPGCameraComponent::UpdateThirdPerson()
{
	auto pEntity = gEnv->pEntitySystem->GetEntity(m_targetEntityID);
	auto pPlayer = CPlayerComponent::GetLocalPlayer();
	auto pPlayerInput = pPlayer->GetPlayerInput();

	if (pPlayerInput)
	{
		// Apply the player input rotation for this frame, and limit the pitch / yaw movement according to the set max and min values.
		if (pPlayer->GetinteractionState().IsCameraMovementAllowed())
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
				m_cameraMatrix = Matrix34::Create(Vec3(1.0f), quatOrbitRotation, vecViewPosition + quatOrbitRotation * m_pCameraManager->GetViewOffset());

				return;
			}
		}
	}

	// The calulation failed somewhere, so just use an default matrix.
	m_cameraMatrix = Matrix34::Create(Vec3(1.0f), IDENTITY, ZERO);

	// If we made it here, we will want to avoid interpolating, since the last frame result will be wrong in some way.
	m_skipInterpolation = true;
}


void CActionRPGCameraComponent::ToggleFirstPerson()
{
	m_isFirstPerson = !m_isFirstPerson;

	// FP pitch is inverse to TP pitch, so we just swap them and clamp them to stop the camera tilting wildly on camera
	// changes. We clamp just in case the two ranges are different or asymetrical.
	m_viewPitch = -m_viewPitch;
	if (m_isFirstPerson)
		m_viewPitch = clamp_tpl(m_viewPitch, DEG2RAD(g_cvars.m_firstPersonCameraPitchMin), DEG2RAD(g_cvars.m_firstPersonCameraPitchMax));
	else
		m_viewPitch = clamp_tpl(m_viewPitch, DEG2RAD(g_cvars.m_actionRPGCameraPitchMin), DEG2RAD(g_cvars.m_actionRPGCameraPitchMax));


	// Notify the local actor that they should change view mode. This gives them a chance to change their model / etc
	// if needed. 
	if (auto pActorComponent = CPlayerComponent::GetLocalActor())
	{
		pActorComponent->OnToggleFirstPerson();
	}
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
	m_EventFlags |= EEntityEvent::Update;
	GetEntity()->UpdateComponentEventMask(this);
	ResetCamera();

	// Avoid interpolation after activating the camera, there is no-where to interpolate from.
	m_skipInterpolation = true;
}


void CActionRPGCameraComponent::OnDeactivate()
{
	m_EventFlags &= ~EEntityEvent::Update;
	GetEntity()->UpdateComponentEventMask(this);
}


// ***
// *** CActionRPGCameraComponent
// ***


void CActionRPGCameraComponent::ResetCamera()
{
	// Default is for zoom level to be as close as possible to fully zoomed in. When toggling from first to third
	// person camera, this gives the least amount of jerking. 
	m_lastZoomGoal = m_zoomGoal = m_zoom = g_cvars.m_actionRPGCameraZoomMin;

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
	Vec3 position {AverageEyePosition};

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
	static IPhysicalEntity* pSkipEnts[10];
	pSkipEnts[0] = gEnv->pEntitySystem->GetEntity(m_targetEntityID)->GetPhysics();

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

CRY_STATIC_AUTO_REGISTER_FUNCTION(&RegisterActionRPGCameraComponent)
}