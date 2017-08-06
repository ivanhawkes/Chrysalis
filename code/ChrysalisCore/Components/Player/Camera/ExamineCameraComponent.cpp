#include <StdAfx.h>

#include "ExamineCameraComponent.h"
#include <Components/Player/Player.h>
#include <Components/Player/Input/IPlayerInputComponent.h>
#include <Actor/Character/Character.h>
#include <Components/Interaction/EntityAwarenessComponent.h>
#include <Console/CVars.h>


namespace Chrysalis
{
void CExamineCameraComponent::Register(Schematyc::CEnvRegistrationScope& componentScope)
{
}


void CExamineCameraComponent::ReflectType(Schematyc::CTypeDesc<CExamineCameraComponent>& desc)
{
	desc.SetGUID(CExamineCameraComponent::IID());
	desc.SetEditorCategory("Cameras");
	desc.SetLabel("Examine Camera");
	desc.SetDescription("No description.");
	desc.SetIcon("icons:General/Camera.ico");
	desc.SetComponentFlags({ IEntityComponent::EFlags::Transform, IEntityComponent::EFlags::Socket, IEntityComponent::EFlags::Attach, IEntityComponent::EFlags::ClientOnly });
}


void CExamineCameraComponent::Initialize()
{
	// It's a good idea to use the entity as a default for our target entity.
	m_targetEntityID = GetEntityId();

	// TODO: CRITICAL: HACK: BROKEN: !!
	// Create a new view and link it to this entity.
	//auto pViewSystem = gEnv->pGameFramework->GetIViewSystem();
	//m_pView = pViewSystem->CreateView();
	//m_pView->LinkTo(GetGameObject());

	// We are usually hosted in the same entity as a camera manager. Use it if you can find one.
	m_pCameraManager = GetEntity()->GetComponent<CCameraManagerComponent>();
}


void CExamineCameraComponent::ProcessEvent(SEntityEvent& event)
{
	switch (event.event)
	{
		case ENTITY_EVENT_UPDATE:
			Update();
			break;
	}
}


bool CExamineCameraComponent::OnAsyncCameraCallback(const HmdTrackingState& sensorState, IHmdDevice::AsyncCameraContext& context)
{
	context.outputCameraMatrix = GetWorldTransformMatrix();

	Matrix33 orientation = Matrix33(context.outputCameraMatrix);
	Vec3 position = context.outputCameraMatrix.GetTranslation();

	context.outputCameraMatrix.AddTranslation(orientation * sensorState.pose.position);
	context.outputCameraMatrix.SetRotation33(orientation * Matrix33(sensorState.pose.orientation));

	return true;
}


void CExamineCameraComponent::OnEntityEvent(IEntity* pEntity, SEntityEvent& event)
{
	switch (event.event)
	{
		case ENTITY_EVENT_DONE:
		{
			break;
		}
	}
}


void CExamineCameraComponent::OnShutDown()
{
	// Release the view.
	// TODO: CRITICAL: HACK: BROKEN: !!
	//GetGameObject()->ReleaseView(this);
	//gEnv->pGameFramework->GetIViewSystem()->RemoveView(m_pView);
	//m_pView = nullptr;
}


void CExamineCameraComponent::Update()
{
	// Default on failure is to return a cleanly constructed blank camera pose.
	CCameraPose newCameraPose { CCameraPose() };

	if (auto pPlayerInput = CPlayerComponent::GetLocalPlayer()->GetPlayerInput())
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
			if (CPlayerComponent::GetLocalPlayer()->GetAllowCameraMovement())
			{
				m_viewYaw -= pPlayerInput->GetMouseYawDelta() - pPlayerInput->GetXiYawDelta();
				m_viewYaw = clamp_tpl(m_viewYaw, DEG2RAD(g_cvars.m_examineCameraYawMin), DEG2RAD(g_cvars.m_examineCameraYawMax));
				m_viewPitch -= pPlayerInput->GetMousePitchDelta() - pPlayerInput->GetXiPitchDelta();
				m_viewPitch = clamp_tpl(m_viewPitch, DEG2RAD(g_cvars.m_examineCameraPitchMin), DEG2RAD(g_cvars.m_examineCameraPitchMax));
			}

			// Pose is based on entity position and the eye position.
			// We will use the rotation of the entity as a base, and apply pitch based on our own reckoning.
			const Vec3 position = pEntity->GetPos();// +xx;
			const Quat rotation = pEntity->GetRotation();// *Quat(Ang3(m_viewPitch, 0.0f, 0.0f)) * Quat(Ang3(m_viewYaw, 0.0f, 0.0f));
			newCameraPose = CCameraPose(position, rotation);

#if defined(_DEBUG)
			if (g_cvars.m_examineCameraDebug)
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
	// TODO: CRITICAL: HACK: BROKEN: !!
	//gEnv->pGameFramework->GetIViewSystem()->SetActiveView(m_pView);
	//GetGameObject()->CaptureView(this);
	m_EventMask |= BIT64(ENTITY_EVENT_UPDATE);

	// HACK: Cheap way to get the entity they wish to examine. This should be refactored to something less fragile.
	if (auto pCharacter = CPlayerComponent::GetLocalCharacter())
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
	// TODO: CRITICAL: HACK: BROKEN: !!
	//GetGameObject()->ReleaseView(this);
	m_EventMask &= ~BIT64(ENTITY_EVENT_UPDATE);
}


// ***
// *** IGameObjectView
// ***


//void CExamineCameraComponent::UpdateView(SViewParams& params)
//{
//	// The last update call should have given us a new updated position and rotation.
//	// We now pass those off to the view system. 
//	params.SaveLast();
//	params.position = GetCameraPose().GetPosition();
//	params.rotation = GetCameraPose().GetRotation();
//
//	// Apply a last minute offset if available for debugging purposes.
//	if (m_pCameraManager)
//		params.position += GetCameraPose().GetRotation() * m_pCameraManager->GetViewOffset();
//
//	// For simplicity at present we are just hard coding the FoV.
//	params.fov = DEG2RAD(60.0f);
//}


// ***
// *** CExamineCameraComponent
// ***


void CExamineCameraComponent::ResetCamera()
{
	m_viewYaw = (DEG2RAD(g_cvars.m_examineCameraYawMax) + DEG2RAD(g_cvars.m_examineCameraYawMin)) / 2;
	m_viewPitch = (DEG2RAD(g_cvars.m_examineCameraPitchMax) + DEG2RAD(g_cvars.m_examineCameraPitchMin)) / 2;
}
}