#include <StdAfx.h>

#include "ActionRPGCameraComponent.h"
#include "Plugin/ChrysalisCorePlugin.h"
#include <IActorSystem.h>
#include <Player/Player.h>
#include <Player/Input/IPlayerInputComponent.h>
#include <Utility/StringConversions.h>


CRYREGISTER_CLASS(CActionRPGCameraComponent)


// TODO: ILH I've dumped this here to get the code to compile again, but it really needs to be switched to whatever
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


class CActionRPGCameraRegistrator : public IEntityRegistrator, public CActionRPGCameraComponent::SExternalCVars
{
	virtual void Register() override
	{
		CChrysalisCorePlugin::RegisterEntityWithDefaultComponent<CActionRPGCameraComponent>("ActionRPGCamera");
		//RegisterEntityWithDefaultComponent<CActionRPGCameraComponent>("ActionRPGCamera", "Camera", "Light.bmp");

		// This should make the entity class invisible in the editor.
		auto cls = gEnv->pEntitySystem->GetClassRegistry()->FindClass("ActionRPGCamera");
		cls->SetFlags(cls->GetFlags() | ECLF_INVISIBLE);

		RegisterCVars();
	}

	void Unregister() override {};

	void RegisterCVars()
	{
		REGISTER_CVAR2("camera_actionrpg_Debug", &m_debug, 0, VF_CHEAT, "Allow debug display.");
		REGISTER_CVAR2("camera_actionrpg_PitchMin", &m_pitchMin, -85.0f, VF_CHEAT, "Minimum pitch angle of the camera (degrees)");
		REGISTER_CVAR2("camera_actionrpg_PitchMax", &m_pitchMax, 85.0f, VF_CHEAT, "Maximum pitch angle of the camera (degrees)");
		REGISTER_CVAR2("camera_actionrpg_TargetDistance", &m_targetDistance, 5.0f, VF_CHEAT, "Preferred distance in metres from the view camera to the target.");
		REGISTER_CVAR2("camera_actionrpg_ReversePitchTilt", &m_reversePitchTilt, 0.1f, VF_CHEAT, "A factor that applies pitch in the direction the camera is facing. The effect of this is to tilt it towards or away from the orbit centre.");
		REGISTER_CVAR2("camera_actionrpg_SlerpSpeed", &m_slerpSpeed, 20.0f, VF_CHEAT, "The slerp speed. The speed at which we apply spherical linear interpolation to the camera rotations.");
		REGISTER_CVAR2("camera_actionrpg_ZoomMin", &m_zoomMin, 0.2f, VF_CHEAT, "The minimum value for camera zoom.");
		REGISTER_CVAR2("camera_actionrpg_ZoomMax", &m_zoomMax, 1.0f, VF_CHEAT, "The maximum value for camera zoom.");
		REGISTER_CVAR2("camera_actionrpg_ZoomStep", &m_zoomStep, 0.08f, VF_CHEAT, "Each zoom event in or out will alter the zoom factor, m_zoom, by this amount. Use lower values for more steps and higher values to zoom in / out faster with less steps.");
		REGISTER_CVAR2("camera_actionrpg_ZoomSpeed", &m_zoomSpeed, 10.0f, VF_CHEAT, "When the zoom changes we interpolate between it's last value and the goal value. This provides for smoother movement on camera zooms. Higher values will interpolate faster than lower values.");

		m_viewPositionOffset = REGISTER_STRING("camera_actionrpg_ViewPositionOffset", "0, 0, 0", VF_CHEAT, "A translation vector which is applied after the camera is initially positioned. This provides for 'over the shoulder' views of the target actor.");
		m_aimPositionOffset = REGISTER_STRING("camera_actionrpg_AimPositionOffset", "0.45, -0.5, 0.0", VF_CHEAT, "A translation vector which is applied after the camera is initially positioned. This provides for 'over the shoulder' views of the target actor.");
	}
};

CActionRPGCameraRegistrator g_actionRPGCameraRegistrator;


// ***
// *** IGameObjectExtension
// ***


void CActionRPGCameraComponent::Initialize()
{
}


void CActionRPGCameraComponent::PostInit(IGameObject * pGameObject)
{
	pGameObject->EnableUpdateSlot(this, 0);

	// Required for 5.3 to call update.
	GetEntity()->Activate(true);

	// It's a good idea to use the entity as a default for our target entity.
	m_targetEntityID = GetEntityId();

	// Create a new view and link it to this entity.
	auto pViewSystem = gEnv->pGameFramework->GetIViewSystem();
	m_pView = pViewSystem->CreateView();
	m_pView->LinkTo(GetGameObject());

	// We are usually hosted in the same entity as a camera manager. Use it if you can find one.
	m_pCameraManager = GetEntity()->GetComponent<CCameraManagerComponent>();
}


void CActionRPGCameraComponent::ProcessEvent(SEntityEvent& event)
{
	switch (event.event)
	{
		case ENTITY_EVENT_UPDATE:
			//Update2();
			break;
	}
}


void CActionRPGCameraComponent::OnShutDown()
{
	// Release the view.
	GetGameObject()->ReleaseView(this);
	gEnv->pGameFramework->GetIViewSystem()->RemoveView(m_pView);
	m_pView = nullptr;
}


void CActionRPGCameraComponent::Update(SEntityUpdateContext& ctx, int updateSlot)
//void CActionRPGCameraComponent::Update2()
{
	// Default on failure is to return a cleanly constructed blank camera pose.
	CCameraPose newCameraPose { CCameraPose() };

	auto pEntity = gEnv->pEntitySystem->GetEntity(m_targetEntityID);
	auto pPlayerInput = CPlayer::GetLocalPlayer()->GetPlayerInput();

	if (pPlayerInput)
	{
		// If the player zoomed all the way in, switch to the first person camera.
		float tempZoomGoal = m_lastZoomGoal + pPlayerInput->GetZoomDelta() * GetCVars().m_zoomStep;
		if (m_pCameraManager)
		{
			if ((tempZoomGoal < GetCVars().m_zoomMin) && (m_pCameraManager->IsThirdPerson()))
				m_pCameraManager->ToggleThirdPerson();
		}

		// Calculate the new zoom goal after asking the player input for zoom level deltas.
		m_zoomGoal = clamp_tpl(tempZoomGoal, GetCVars().m_zoomMin, GetCVars().m_zoomMax);
		m_lastZoomGoal = m_zoomGoal;

		// Apply the player input rotation for this frame, and limit the pitch / yaw movement according to the set max and min values.
		m_viewPitch += pPlayerInput->GetMousePitchDelta() + pPlayerInput->GetXiPitchDelta();
		m_viewPitch = clamp_tpl(m_viewPitch, DEG2RAD(GetCVars().m_pitchMin), DEG2RAD(GetCVars().m_pitchMax));
		m_viewYaw += pPlayerInput->GetMouseYawDelta() - pPlayerInput->GetXiYawDelta();

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
			Interpolate(m_zoom, m_zoomGoal, GetCVars().m_zoomSpeed, gEnv->pTimer->GetFrameTime());

			// Get the entity we are targeting.
			auto pTargetEntity = gEnv->pEntitySystem->GetEntity(m_targetEntityID);
			if (pTargetEntity)
			{
				// Calculate pitch and yaw movements to apply both prior to and after positioning the camera.
				Quat quatPreTransYP = Quat(Ang3(m_viewPitch, 0.0f, m_viewYaw));
				Quat quatPostTransYP = Quat::CreateRotationXYZ(Ang3(m_viewPitch * GetCVars().m_reversePitchTilt, 0.0f, 0.0f));

				// Target and aim position come from the entity position.
				Vec3 vecTargetAimPosition = GetTargetAimPosition(pTargetEntity);

				// The distance from the view to the target.
				float shapedZoom = (m_zoom * m_zoom) / (GetCVars().m_zoomMax * GetCVars().m_zoomMax);
				float zoomDistance = GetCVars().m_targetDistance * shapedZoom;

				// Calculate the target rotation and slerp it if we can.
				Quat quatTargetRotationGoal = m_quatTargetRotation * quatPreTransYP;
				Quat quatTargetRotation;
				if (m_skipInterpolation)
				{
					m_quatLastTargetRotation = quatTargetRotation = quatTargetRotationGoal;
				}
				else
				{
					quatTargetRotation = Quat::CreateSlerp(m_quatLastTargetRotation, quatTargetRotationGoal, gEnv->pTimer->GetFrameTime() * GetCVars().m_slerpSpeed);
					m_quatLastTargetRotation = quatTargetRotation;
				}

				// Work out where to place the new initial position. We will be using a unit vector facing forward Y
				// as the starting place and applying rotations from the target bone and player camera movements.
				Vec3 viewPositionOffset = Vec3FromString(GetCVars().m_viewPositionOffset->GetString());
				Vec3 vecViewPosition = vecTargetAimPosition + (quatTargetRotation * (Vec3(0.0f, 1.0f, 0.0f) * zoomDistance))
					+ quatTargetRotation * viewPositionOffset;

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
					quatViewRotation = Quat::CreateSlerp(m_quatLastViewRotation, quatViewRotationGoal, gEnv->pTimer->GetFrameTime() * GetCVars().m_slerpSpeed);
					m_quatLastViewRotation = quatViewRotation;
				}

				// Apply a final translation to both the view position and the aim position.
				Vec3 aimPositionOffset = Vec3FromString(GetCVars().m_aimPositionOffset->GetString());
				vecViewPosition += quatViewRotation * aimPositionOffset;

				// Gimbal style rotation after it's moved into it's initial position.
				Quat quatOrbitRotation = quatViewRotation * quatPostTransYP;

				// Perform a collision detection. Note, any collisions will disallow use of interpolated camera movement.
				CollisionDetection(vecTargetAimPosition, vecViewPosition);

#if defined(_DEBUG)
				if (GetCVars().m_debug)
				{
					gEnv->pRenderer->GetIRenderAuxGeom()->DrawSphere(vecTargetAimPosition, 0.04f, ColorB(128, 0, 0));
					gEnv->pRenderer->GetIRenderAuxGeom()->DrawSphere(vecTargetAimPosition + quatTargetRotation * viewPositionOffset, 0.04f, ColorB(0, 128, 0));
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
	}

	// We set the pose, regardless of the result.
	SetCameraPose(newCameraPose);

	// If we made it here, we will want to avoid interpolating, since the last frame result will be wrong in some way.
	m_skipInterpolation = true;
}


// ***
// *** IGameObjectView
// ***


void CActionRPGCameraComponent::UpdateView(SViewParams& params)
{
	// The last update call should have given us a new updated position and rotation.
	// We now pass those off to the view system. 
	params.SaveLast();
	params.position = GetCameraPose().GetPosition();
	params.rotation = GetCameraPose().GetRotation();

	// Apply a last minute offset if available for debugging purposes.
	if (m_pCameraManager)
		params.position += GetCameraPose().GetRotation() * m_pCameraManager->GetViewOffset();

	// For simplicity at present we are just hard coding the FoV.
	params.fov = DEG2RAD(60.0f);
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
	gEnv->pGameFramework->GetIViewSystem()->SetActiveView(m_pView);
	GetGameObject()->CaptureView(this);
	GetGameObject()->EnableUpdateSlot(this, CPlayer::EPlayerUpdateSlot::ePlayerUpdateSlot_CameraThirdPerson);
	m_EventMask |= BIT64(ENTITY_EVENT_UPDATE);
}


void CActionRPGCameraComponent::OnDeactivate()
{
	GetGameObject()->ReleaseView(this);
	GetGameObject()->DisableUpdateSlot(this, CPlayer::EPlayerUpdateSlot::ePlayerUpdateSlot_CameraThirdPerson);
	m_EventMask &= ~BIT64(ENTITY_EVENT_UPDATE);
}


// ***
// *** CActionRPGCameraComponent
// ***


const CActionRPGCameraComponent::SExternalCVars& CActionRPGCameraComponent::GetCVars() const
{
	return g_actionRPGCameraRegistrator;
}


void CActionRPGCameraComponent::ResetCamera()
{
	// Zoom can default to it's mid-point.
	m_lastZoomGoal = m_zoomGoal = m_zoom = (GetCVars().m_zoomMax + GetCVars().m_zoomMin) / 2;

	// Pitch and yaw can default to their mid-points.
	m_viewPitch = (DEG2RAD(GetCVars().m_pitchMax) + DEG2RAD(GetCVars().m_pitchMin)) / 2;

	// We'll start from behind our character as a default.
	m_viewYaw = DEG2RAD(180.0f);
}


Vec3 CActionRPGCameraComponent::GetTargetAimPosition(IEntity* const pEntity)
{
	Vec3 position { AverageEyePosition };

	if (pEntity)
	{
		// If we are attached to an entity that is an actor we can use their eye position.
		Vec3 localEyePosition = position;
		auto pActor = gEnv->pGameFramework->GetIActorSystem()->GetActor(m_targetEntityID);
		if (pActor)
			localEyePosition = pActor->GetLocalEyePos();

		// Pose is based on entity position and the eye position.
		// We will use the rotation of the entity as a base, and apply pitch based on our own reckoning.
		position = pEntity->GetPos() + localEyePosition;

#if defined(_DEBUG)
		if (GetCVars().m_debug)
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
