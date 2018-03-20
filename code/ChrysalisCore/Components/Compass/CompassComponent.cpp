#include "StdAfx.h"

#include "CompassComponent.h"


namespace Chrysalis
{
void CCompassComponent::Register(Schematyc::CEnvRegistrationScope& componentScope)
{
}


void CCompassComponent::ReflectType(Schematyc::CTypeDesc<CCompassComponent>& desc)
{
	desc.SetGUID(CCompassComponent::IID());
	desc.SetEditorCategory("Compass");
	desc.SetLabel("Compass");
	desc.SetDescription("No description.");
	desc.SetIcon("icons:ObjectTypes/light.ico");
	desc.SetComponentFlags({ IEntityComponent::EFlags::Transform });

	// Mark the entity interaction component as a hard requirement.
	desc.AddComponentInteraction(SEntityComponentRequirements::EType::HardDependency, CEntityInteractionComponent::IID());
}


void CCompassComponent::Initialize()
{
	// Get some geometry.
	m_pGeometryComponent = GetEntity()->GetOrCreateComponent<Cry::DefaultComponents::CStaticMeshComponent>();

	//// Get a controllable animation component.
	//m_pControlledAnimationComponent = GetEntity()->CreateComponent<CControlledAnimationComponent>();

	// We want to supply interaction verbs.
	m_interactor = GetEntity()->GetOrCreateComponent<CEntityInteractionComponent>();
	if (m_interactor)
	{
		m_interactor->AddInteraction(std::make_shared<CInteractionItemInspect>(this));
		m_interactor->AddInteraction(std::make_shared<CInteractionItemPickup>(this));
		m_interactor->AddInteraction(std::make_shared<CInteractionItemDrop>(this));
	}
}


void CCompassComponent::OnResetState()
{
}
}