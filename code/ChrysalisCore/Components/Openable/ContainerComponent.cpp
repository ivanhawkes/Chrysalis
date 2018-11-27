#include <StdAfx.h>

#include "ContainerComponent.h"
#include <Components/Interaction/EntityInteractionComponent.h>
#include <DefaultComponents/Geometry/StaticMeshComponent.h>
#include <Components/Animation/SimpleAnimationComponent.h>
#include <Components/Lockable/LockableComponent.h>


namespace Chrysalis
{
void CContainerComponent::Register(Schematyc::CEnvRegistrationScope& componentScope)
{
}


void CContainerComponent::ReflectType(Schematyc::CTypeDesc<CContainerComponent>& desc)
{
	desc.SetGUID(CContainerComponent::IID());
	desc.SetEditorCategory("Containers");
	desc.SetLabel("Container");
	desc.SetDescription("Chests, bags, etc.");
	desc.SetIcon("icons:ObjectTypes/light.ico");
	desc.SetComponentFlags({ IEntityComponent::EFlags::Singleton });

	// Mark the entity interaction component as a hard requirement.
	desc.AddComponentInteraction(SEntityComponentRequirements::EType::HardDependency, CEntityInteractionComponent::IID());
}


void CContainerComponent::Initialize()
{
	// Get some geometry.
	m_pGeometryComponent = m_pEntity->GetOrCreateComponent<Cry::DefaultComponents::CStaticMeshComponent>();

	// Get a simple animation component.
	m_pSimpleAnimationComponent = m_pEntity->GetOrCreateComponent<CSimpleAnimationComponent>();

	// Allow locking.
	m_lockableComponent = m_pEntity->GetOrCreateComponent<CLockableComponent>();

	// We want to supply interaction verbs.
	m_interactor = m_pEntity->GetOrCreateComponent<CEntityInteractionComponent>();
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
}