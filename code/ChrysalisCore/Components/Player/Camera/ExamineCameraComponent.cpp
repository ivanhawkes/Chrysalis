#include <StdAfx.h>

#include "ExamineCameraComponent.h"
#include <CryCore/StaticInstanceList.h>
#include "CrySchematyc/Env/Elements/EnvComponent.h"
#include "CrySchematyc/Env/IEnvRegistrar.h"
#include <CryRenderer/IRenderAuxGeom.h>
#include "Components/Player/PlayerComponent.h"
#include <Components/Player/Input/PlayerInputComponent.h>
#include <Components/Interaction/EntityAwarenessComponent.h>
#include <Console/CVars.h>


namespace Chrysalis
{
static void RegisterExamineCameraComponent(Schematyc::IEnvRegistrar& registrar)
{
	Schematyc::CEnvRegistrationScope scope = registrar.Scope(IEntity::GetEntityScopeGUID());
	{
		Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(CExamineCameraComponent));
		// Functions
		{
		}
	}
}


void CExamineCameraComponent::ReflectType(Schematyc::CTypeDesc<CExamineCameraComponent>& desc)
{
	desc.SetGUID(CExamineCameraComponent::IID());
	desc.SetEditorCategory("Cameras");
	desc.SetLabel("Examine Camera");
	desc.SetDescription("No description.");
	desc.SetIcon("icons:ObjectTypes/light.ico");
	desc.SetComponentFlags({IEntityComponent::EFlags::Socket, IEntityComponent::EFlags::Attach, IEntityComponent::EFlags::ClientOnly});
}


void CExamineCameraComponent::Initialize()
{
	// It's a good idea to use the entity as a default for our target entity.
	m_targetEntityID = GetEntityId();

	// We are usually hosted in the same entity as a camera manager. Use it if you can find one.
	m_pCameraManager = GetEntity()->GetComponent<CCameraManagerComponent>();
}


void CExamineCameraComponent::ProcessEvent(const SEntityEvent& event)
{
	switch (event.event)
	{
		case EEntityEvent::Update:
			Update();
			UpdateView();
			break;
	}
}


void CExamineCameraComponent::OnShutDown()
{
}


void CExamineCameraComponent::Update()
{
	// Default on failure is to return a cleanly constructed default matrix.
	Matrix34 newCameraMatrix = Matrix34::Create(Vec3(1.0f), IDENTITY, ZERO);

	if (auto pPlayerInput = CPlayerComponent::GetLocalPlayer()->GetPlayerInput())
	{
		// HACK: Cheap way to break out of camera mode while developing it - since it totally breaks the view each time.
		if (m_pCameraManager)
		{
			// If we zoom, then cancel this camera mode.
			if (m_pCameraManager->GetZoomDelta() != 0.0f)
				m_pCameraManager->SetCameraMode(ECameraMode::eCameraMode_FirstPerson, "Zoomed out of examine mode");
		}

		// Resolve the entity.
		auto pEntity = gEnv->pEntitySystem->GetEntity(m_targetEntityID);
		if (pEntity)
		{
			// Apply the player input rotation for this frame, and limit the pitch / yaw movement according to the set max and min values.
			if (CPlayerComponent::GetLocalPlayer()->GetinteractionState().IsCameraMovementAllowed())
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
			newCameraMatrix = Matrix34::Create(Vec3(1.0f), rotation, position);

#if defined(_DEBUG)
			if (g_cvars.m_examineCameraDebug)
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


void CExamineCameraComponent::AttachToEntity(EntityId entityId)
{
	m_targetEntityID = entityId;
}


void CExamineCameraComponent::OnActivate()
{
	m_EventFlags |= EEntityEvent::Update;
	GetEntity()->UpdateComponentEventMask(this);
	ResetCamera();

	// HACK: Cheap way to get the entity they wish to examine. This should be refactored to something less fragile.
	if (auto pActorComponent = CPlayerComponent::GetLocalActor())
	{
		if (auto pEntityAwarenessComponent = pActorComponent->GetEntity()->GetComponent<CEntityAwarenessComponent>())
		{
			auto entityId = pEntityAwarenessComponent->GetLookAtEntityId();
			AttachToEntity(entityId);
		}
	}
}


void CExamineCameraComponent::OnDeactivate()
{
	m_EventFlags &= ~EEntityEvent::Update;
	GetEntity()->UpdateComponentEventMask(this);
}


void CExamineCameraComponent::ResetCamera()
{
	m_viewYaw = (DEG2RAD(g_cvars.m_examineCameraYawMax) + DEG2RAD(g_cvars.m_examineCameraYawMin)) / 2;
	m_viewPitch = (DEG2RAD(g_cvars.m_examineCameraPitchMax) + DEG2RAD(g_cvars.m_examineCameraPitchMin)) / 2;
}

CRY_STATIC_AUTO_REGISTER_FUNCTION(&RegisterExamineCameraComponent)
}