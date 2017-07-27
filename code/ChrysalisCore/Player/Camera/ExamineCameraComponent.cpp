#include <StdAfx.h>

#include "ExamineCameraComponent.h"
#include <IActorSystem.h>
#include <Player/Player.h>
#include <Player/Input/IPlayerInputComponent.h>
#include <Actor/Character/Character.h>
#include <Components/Interaction/EntityAwarenessComponent.h>


CRYREGISTER_CLASS(CExamineCameraComponent)


class CExamineCameraRegistrator : public IEntityRegistrator, public CExamineCameraComponent::SExternalCVars
{
	virtual void Register() override
	{
		CChrysalisCorePlugin::RegisterEntityWithDefaultComponent<CExamineCameraComponent>("ExamineCamera");
		//RegisterEntityWithDefaultComponent<CExamineCameraComponent>("ExamineCamera", "Camera", "Camera.bmp");

		// This should make the entity class invisible in the editor.
		auto cls = gEnv->pEntitySystem->GetClassRegistry()->FindClass("ExamineCamera");
		cls->SetFlags(cls->GetFlags() | ECLF_INVISIBLE);

		RegisterCVars();
	}

	void RegisterCVars()
	{
		REGISTER_CVAR2("camera_Examine_Debug", &m_debug, 0, VF_CHEAT, "Allow debug display.");
		REGISTER_CVAR2("camera_Examine_YawMin", &m_yawMin, -85.0f, VF_CHEAT, "Minimum yaw angle of the camera (degrees)");
		REGISTER_CVAR2("camera_Examine_YawMax", &m_yawMax, 85.0f, VF_CHEAT, "Maximum yaw angle of the camera (degrees)");
		REGISTER_CVAR2("camera_Examine_PitchMin", &m_pitchMin, -85.0f, VF_CHEAT, "Minimum pitch angle of the camera (degrees)");
		REGISTER_CVAR2("camera_Examine_PitchMax", &m_pitchMax, 85.0f, VF_CHEAT, "Maximum pitch angle of the camera (degrees)");
	}
};

CExamineCameraRegistrator g_ExamineCameraRegistrator;

// ***
// *** IGameObjectExtension
// ***


void CExamineCameraComponent::Initialize()
{
}


void CExamineCameraComponent::PostInit(IGameObject * pGameObject)
{
	//pGameObject->EnableUpdateSlot(this, CPlayer::EPlayerUpdateSlot::ePlayerUpdateSlot_CameraExamine);

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


void CExamineCameraComponent::ProcessEvent(SEntityEvent& event)
{
	switch (event.event)
	{
		case ENTITY_EVENT_UPDATE:
			//Update2();
			break;
	}
}


void CExamineCameraComponent::OnShutDown()
{
	// Release the view.
	GetGameObject()->ReleaseView(this);
	gEnv->pGameFramework->GetIViewSystem()->RemoveView(m_pView);
	m_pView = nullptr;
}


void CExamineCameraComponent::Update(SEntityUpdateContext& ctx, int updateSlot)
{
	// Default on failure is to return a cleanly constructed blank camera pose.
	CCameraPose newCameraPose { CCameraPose() };

	if (auto pPlayerInput = CPlayer::GetLocalPlayer()->GetPlayerInput())
	{
		// HACK: Cheap way to break out of camera mode while developing it - since it totally breaks the view each time.
		if (m_pCameraManager)
		{
			// If we move or zoom, then cancel this camera mode.
			if ((pPlayerInput->GetZoomDelta() != 0.0f) || (pPlayerInput->GetMovement(m_cameraPose.GetRotation()) != Vec3(IDENTITY)))
				m_pCameraManager->SetCameraMode(ECameraMode::eCameraMode_FirstPerson, "Zoomed out of examine mode");
		}

		// Resolve the entity.
		auto pEntity = gEnv->pEntitySystem->GetEntity(m_targetEntityID);
		if (pEntity)
		{
			// Apply the player input rotation for this frame, and limit the pitch / yaw movement according to the set max and min values.
			if (CPlayer::GetLocalPlayer()->GetAllowCameraMovement())
			{
				m_viewYaw -= pPlayerInput->GetMouseYawDelta() - pPlayerInput->GetXiYawDelta();
				m_viewYaw = clamp_tpl(m_viewYaw, DEG2RAD(GetCVars().m_yawMin), DEG2RAD(GetCVars().m_yawMax));
				m_viewPitch -= pPlayerInput->GetMousePitchDelta() - pPlayerInput->GetXiPitchDelta();
				m_viewPitch = clamp_tpl(m_viewPitch, DEG2RAD(GetCVars().m_pitchMin), DEG2RAD(GetCVars().m_pitchMax));
			}

			// Pose is based on entity position and the eye position.
			// We will use the rotation of the entity as a base, and apply pitch based on our own reckoning.
			const Vec3 position = pEntity->GetPos();// +xx;
			const Quat rotation = pEntity->GetRotation();// *Quat(Ang3(m_viewPitch, 0.0f, 0.0f)) * Quat(Ang3(m_viewYaw, 0.0f, 0.0f));
			newCameraPose = CCameraPose(position, rotation);

#if defined(_DEBUG)
			if (GetCVars().m_debug)
			{
				gEnv->pRenderer->GetIRenderAuxGeom()->DrawSphere(position, 0.04f, ColorB(0, 0, 255, 255));
			}
#endif
		}
	}

	// We set the pose, regardless of the result.
	SetCameraPose(newCameraPose);
}


// ***
// *** ICameraComponent
// ***


void CExamineCameraComponent::AttachToEntity(EntityId entityId)
{
	m_targetEntityID = entityId; 
}


void CExamineCameraComponent::OnActivate()
{
	gEnv->pGameFramework->GetIViewSystem()->SetActiveView(m_pView);
	GetGameObject()->CaptureView(this);
	GetGameObject()->EnableUpdateSlot(this, CPlayer::EPlayerUpdateSlot::ePlayerUpdateSlot_CameraExamine);
	m_EventMask |= BIT64(ENTITY_EVENT_UPDATE);
	
	// HACK: Cheap way to get the entity they wish to examine. This should be refactored to something less fragile.
	if (auto pCharacter = CPlayer::GetLocalCharacter())
	{
		if (auto pEntityAwarenessComponent = pCharacter->GetEntity()->GetComponent<CEntityAwarenessComponent>())
		{
			auto entityId = pEntityAwarenessComponent->GetLookAtEntityId();
			AttachToEntity(entityId);
		}
	}
}


void CExamineCameraComponent::OnDeactivate()
{
	GetGameObject()->ReleaseView(this);
	GetGameObject()->DisableUpdateSlot(this, CPlayer::EPlayerUpdateSlot::ePlayerUpdateSlot_CameraExamine);
	m_EventMask &= ~BIT64(ENTITY_EVENT_UPDATE);
}


// ***
// *** IGameObjectView
// ***


void CExamineCameraComponent::UpdateView(SViewParams& params)
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
// *** CExamineCameraComponent
// ***


const CExamineCameraComponent::SExternalCVars& CExamineCameraComponent::GetCVars() const
{
	return g_ExamineCameraRegistrator;
}

void CExamineCameraComponent::ResetCamera()
{
	m_viewYaw = (DEG2RAD(GetCVars().m_yawMax) + DEG2RAD(GetCVars().m_yawMin)) / 2;
	m_viewPitch = (DEG2RAD(GetCVars().m_pitchMax) + DEG2RAD(GetCVars().m_pitchMin)) / 2;
}
