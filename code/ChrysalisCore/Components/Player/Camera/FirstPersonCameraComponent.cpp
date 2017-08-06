#include <StdAfx.h>

#include "FirstPersonCameraComponent.h"
#include <Components/Player/Player.h>
#include <Components/Player/Input/IPlayerInputComponent.h>
#include <Console/CVars.h>


namespace Chrysalis
{
void CFirstPersonCameraComponent::Register(Schematyc::CEnvRegistrationScope& componentScope)
{
}


void CFirstPersonCameraComponent::ReflectType(Schematyc::CTypeDesc<CFirstPersonCameraComponent>& desc)
{
	desc.SetGUID(CFirstPersonCameraComponent::IID());
	desc.SetEditorCategory("Cameras");
	desc.SetLabel("First Person Camera");
	desc.SetDescription("A first person camera which attaches to a target.");
	desc.SetIcon("icons:General/Camera.ico");
	desc.SetComponentFlags({ IEntityComponent::EFlags::Transform, IEntityComponent::EFlags::Socket, IEntityComponent::EFlags::Attach, IEntityComponent::EFlags::ClientOnly });
}


void CFirstPersonCameraComponent::Initialize()
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


void CFirstPersonCameraComponent::ProcessEvent(SEntityEvent& event)
{
	switch (event.event)
	{
		case ENTITY_EVENT_UPDATE:
			Update();
			break;
	}
}


bool CFirstPersonCameraComponent::OnAsyncCameraCallback(const HmdTrackingState& sensorState, IHmdDevice::AsyncCameraContext& context)
{
	context.outputCameraMatrix = GetWorldTransformMatrix();

	Matrix33 orientation = Matrix33(context.outputCameraMatrix);
	Vec3 position = context.outputCameraMatrix.GetTranslation();

	context.outputCameraMatrix.AddTranslation(orientation * sensorState.pose.position);
	context.outputCameraMatrix.SetRotation33(orientation * Matrix33(sensorState.pose.orientation));

	return true;
}


void CFirstPersonCameraComponent::OnEntityEvent(IEntity* pEntity, SEntityEvent& event)
{
	switch (event.event)
	{
		case ENTITY_EVENT_DONE:
		{
			break;
		}
	}
}


void CFirstPersonCameraComponent::OnShutDown()
{
	// Release the view.
	// TODO: CRITICAL: HACK: BROKEN: !!
	//GetGameObject()->ReleaseView(this);
	//gEnv->pGameFramework->GetIViewSystem()->RemoveView(m_pView);
	//m_pView = nullptr;
}


void CFirstPersonCameraComponent::Update()
{
	auto pPlayer = CPlayerComponent::GetLocalPlayer();
	auto pPlayerInput = pPlayer->GetPlayerInput();

	// Default on failure is to return a cleanly constructed blank camera pose.
	CCameraPose newCameraPose { CCameraPose() };

	if (pPlayerInput)
	{
		// If the player changes the camera zoom, we will toggle to the third person view.
		if (m_pCameraManager)
		{
			if ((pPlayerInput->GetZoomDelta() > FLT_EPSILON) && (!m_pCameraManager->IsThirdPerson()))
				m_pCameraManager->ToggleThirdPerson();
		}

		// Resolve the entity.
		auto pEntity = gEnv->pEntitySystem->GetEntity(m_targetEntityID);
		if (pEntity)
		{
			// It's possible there is no actor to query for eye position, in that case, return a safe default
			// value for an average height person.
			Vec3 localEyePosition { AverageEyePosition };

			// If we are attached to an entity that is an actor we can use their eye position.
			auto pActor = CActor::GetActor(m_targetEntityID);
			if (pActor)
				localEyePosition = pActor->GetLocalEyePos();

			// Apply the player input rotation for this frame, and limit the pitch / yaw movement according to the set max and min values.
			if (pPlayer->GetAllowCameraMovement())
			{
				m_viewPitch -= pPlayerInput->GetMousePitchDelta() - pPlayerInput->GetXiPitchDelta();
				m_viewPitch = clamp_tpl(m_viewPitch, DEG2RAD(g_cvars.m_firstPersonCameraPitchMin), DEG2RAD(g_cvars.m_firstPersonCameraPitchMax));
			}

			// Pose is based on entity position and the eye position.
			// We will use the rotation of the entity as a base, and apply pitch based on our own reckoning.
			const Vec3 position = pEntity->GetPos() + localEyePosition;
			const Quat rotation = pEntity->GetRotation() * Quat(Ang3(m_viewPitch, 0.0f, 0.0f));
			newCameraPose = CCameraPose(position, rotation);

#if defined(_DEBUG)
			if (g_cvars.m_firstPersonCameraDebug)
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


void CFirstPersonCameraComponent::OnActivate()
{
	//gEnv->pGameFramework->GetIViewSystem()->SetActiveView(m_pView);
	// TODO: CRITICAL: HACK: BROKEN: !!
	//GetGameObject()->CaptureView(this);
	m_EventMask |= BIT64(ENTITY_EVENT_UPDATE);
}


void CFirstPersonCameraComponent::OnDeactivate()
{
	// TODO: CRITICAL: HACK: BROKEN: !!
	//GetGameObject()->ReleaseView(this);
	m_EventMask &= ~BIT64(ENTITY_EVENT_UPDATE);
}


//void CFirstPersonCameraComponent::UpdateView(SViewParams& params)
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
}