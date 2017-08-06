#include "StdAfx.h"

#include "SecurityPadComponent.h"
#include <Components/Player/Player.h>
#include <Components/Player/Camera/CameraManagerComponent.h>


namespace Chrysalis
{
void CSecurityPadComponent::Register(Schematyc::CEnvRegistrationScope& componentScope)
{
}


void CSecurityPadComponent::ReflectType(Schematyc::CTypeDesc<CSecurityPadComponent>& desc)
{
	desc.SetGUID(CSecurityPadComponent::IID());
	desc.SetEditorCategory("Interactive");
	desc.SetLabel("Security Pad");
	desc.SetDescription("Interactive security pad for controlling access to locked areas.");
	desc.SetIcon("icons:ObjectTypes/light.ico");
	desc.SetComponentFlags({ IEntityComponent::EFlags::Transform });
}


void CSecurityPadComponent::Initialize()
{
	// Get some geometry.
	m_pGeometryComponent = GetEntity()->CreateComponent<Cry::DefaultComponents::CStaticMeshComponent>();

	// We want to supply interaction verbs.
	m_interactor = GetEntity()->GetOrCreateComponent<CEntityInteractionComponent>();
	if (m_interactor)
	{
		m_interactor->AddInteraction(std::make_shared<CInteractionExamine>(this));
	}
}


void CSecurityPadComponent::OnResetState()
{
}


void CSecurityPadComponent::OnInteractionExamineStart()
{
	gEnv->pLog->LogAlways("OnInteractionExamineStart fired.");

	// Switch to the examine camera.
	if (auto pPlayer = CPlayerComponent::GetLocalPlayer())
	{
		auto pCameraManager = pPlayer->GetCameraManager();
		pCameraManager->SetCameraMode(ECameraMode::eCameraMode_Examine, "Examine entity");
	}
}


void CSecurityPadComponent::OnInteractionExamineComplete()
{
	gEnv->pLog->LogAlways("OnInteractionExamineComplete fired.");

	// Switch back to the previous camera.
	if (auto pPlayer = CPlayerComponent::GetLocalPlayer())
	{
		auto pCameraManager = pPlayer->GetCameraManager();
		pCameraManager->SetLastCameraMode();
	}
}


void CSecurityPadComponent::OnInteractionExamineCancel()
{
	// Same result as completing.
	OnInteractionExamineComplete();
}
}