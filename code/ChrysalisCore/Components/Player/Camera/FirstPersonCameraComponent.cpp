#include <StdAfx.h>

#include "FirstPersonCameraComponent.h"
#include "Components/Player/PlayerComponent.h"
#include <Components/Player/Input/PlayerInputComponent.h>
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
	desc.SetIcon("icons:ObjectTypes/light.ico");
	desc.SetComponentFlags({ IEntityComponent::EFlags::Socket, IEntityComponent::EFlags::Attach, IEntityComponent::EFlags::ClientOnly });
}


void CFirstPersonCameraComponent::Initialize()
{
	// It's a good idea to use the entity as a default for our target entity.
	m_targetEntityID = GetEntityId();

	// We are usually hosted in the same entity as a camera manager. Use it if you can find one.
	m_pCameraManager = GetEntity()->GetComponent<CCameraManagerComponent>();
}


void CFirstPersonCameraComponent::ProcessEvent(const SEntityEvent& event)
{
	switch (event.event)
	{
		case EEntityEvent::Update:
			Update();
			UpdateView();
			break;
	}
}


void CFirstPersonCameraComponent::OnShutDown()
{
}


void CFirstPersonCameraComponent::Update()
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
			Vec3 localEyePosition { AverageEyePosition };

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


// ***
// *** ICameraComponent
// ***


void CFirstPersonCameraComponent::OnActivate()
{
	m_EventMask |= EventToMask(EEntityEvent::Update);
	GetEntity()->UpdateComponentEventMask(this);
	ResetCamera();
}


void CFirstPersonCameraComponent::OnDeactivate()
{
	m_EventMask &= ~EventToMask(EEntityEvent::Update);
	GetEntity()->UpdateComponentEventMask(this);
}


void CFirstPersonCameraComponent::ResetCamera()
{
	m_viewPitch = (DEG2RAD(g_cvars.m_firstPersonCameraPitchMax) + DEG2RAD(g_cvars.m_firstPersonCameraPitchMin)) / 2;
}
}