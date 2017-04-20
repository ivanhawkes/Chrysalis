#include <StdAfx.h>

#include "ContainerComponent.h"
#include <Components/Interaction/EntityInteractionComponent.h>
#include <Components/Geometry/GeometryComponent.h>
#include <Components/Animation/SimpleAnimationComponent.h>
#include <Components/Lockable/LockableComponent.h>


CRYREGISTER_CLASS(CContainerComponent)


class CContainerExtensionRegistrator : public IEntityRegistrator, public CContainerComponent::SExternalCVars
{
	virtual void Register() override
	{
		RegisterEntityWithDefaultComponent<CContainerComponent>("Container", "Containers", "door.bmp");

		RegisterCVars();
	}

	void RegisterCVars()
	{
		REGISTER_CVAR2("entity_containerextension_Debug", &m_debug, 0, VF_CHEAT, "Allow debug display.");
	}
};

CContainerExtensionRegistrator g_ContainerExtensionRegistrator;

const CContainerComponent::SExternalCVars& CContainerComponent::GetCVars() const
{
	return g_ContainerExtensionRegistrator;
}


void CContainerComponent::Initialize()
{
	auto pEntity = GetEntity();

	// Get some geometry.
	m_pGeometryComponent = pEntity->CreateComponent<CGeometryComponent>();

	// Get a simple animation component.
	m_pSimpleAnimationComponent = pEntity->CreateComponent<CSimpleAnimationComponent>();

	// Allow locking.
	m_lockableComponent = pEntity->CreateComponent<CLockableComponent>();

	// We want to supply interaction verbs.
	m_interactor = pEntity->GetOrCreateComponent<CEntityInteractionComponent>();
	if (m_interactor)
	{
		m_interactor->AddInteraction(std::make_shared<CInteractionOpenableOpen>(this));
		m_interactor->AddInteraction(std::make_shared<CInteractionOpenableClose>(this));
		m_interactor->AddInteraction(std::make_shared<CInteractionLockableLock>(this));
		m_interactor->AddInteraction(std::make_shared<CInteractionLockableUnlock>(this));
	}

	OnResetState();
}


void CContainerComponent::OnResetState()
{
}


void CContainerComponent::SerializeProperties(Serialization::IArchive& archive)
{
	if (archive.isInput())
	{
		OnResetState();
	}
}
