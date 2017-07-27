#include "StdAfx.h"

#include "SecurityPadComponent.h"
#include <CrySerialization/Decorators/Resources.h>
#include <CrySerialization/Enum.h>
#include <Player/Player.h>
#include <Player/Camera/CameraManagerComponent.h>


class CSecurityPadEntityRegistrator : public IEntityRegistrator
{
	virtual void Register() override
	{
		RegisterEntityWithDefaultComponent<CSecurityPadComponent>("SecurityPadComponent", "SecurityPad", "physicsobject.bmp", true);
	}
};

CSecurityPadEntityRegistrator g_SecurityPadEntityRegistrator;

CRYREGISTER_CLASS(CSecurityPadComponent);


void CSecurityPadComponent::Initialize()
{
	// Get some geometry.
	m_pGeometryComponent = GetEntity()->CreateComponent<CGeometryComponent>();

	// We want to supply interaction verbs.
	m_interactor = GetEntity()->GetOrCreateComponent<CEntityInteractionComponent>();
	if (m_interactor)
	{
		m_interactor->AddInteraction(std::make_shared<CInteractionExamine>(this));
	}

	CDesignerEntityComponent::Initialize();
}


void CSecurityPadComponent::SerializeProperties(Serialization::IArchive& archive)
{
	if (archive.isInput())
	{
		OnResetState();
	}
}


void CSecurityPadComponent::OnResetState()
{
}


void CSecurityPadComponent::OnInteractionExamineStart()
{
	gEnv->pLog->LogAlways("OnInteractionExamineStart fired.");
	
	// Switch to the examine camera.
	if (auto pPlayer = CPlayer::GetLocalPlayer())
	{
		auto pCameraManager = pPlayer->GetCameraManager();
		pCameraManager->SetCameraMode(ECameraMode::eCameraMode_Examine, "Examine entity");
	}
}


void CSecurityPadComponent::OnInteractionExamineComplete()
{
	gEnv->pLog->LogAlways("OnInteractionExamineComplete fired.");

	// Switch back to the previous camera.
	if (auto pPlayer = CPlayer::GetLocalPlayer())
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
