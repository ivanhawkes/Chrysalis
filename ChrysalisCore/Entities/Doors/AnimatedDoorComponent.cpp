#include <StdAfx.h>

#include "AnimatedDoorComponent.h"
#include <Entities/Lockable/LockableComponent.h>
#include <CrySerialization/Decorators/Resources.h>


CRYREGISTER_CLASS(CAnimatedDoorComponent)

class CDoorRegistrator : public IEntityRegistrator
{
	void Register() override
	{
		RegisterEntityWithDefaultComponent<CAnimatedDoorComponent>("AnimatedDoor", "Doors", "Light.bmp");
	}
};

CDoorRegistrator g_doorRegistrator;


void CAnimatedDoorComponent::Initialize()
{
	auto pEntity = GetEntity();
	
	m_pGeometryComponent = GetEntity()->GetOrCreateComponent<CGeometryComponent>();

	auto m_lockableExtension = pEntity->GetOrCreateComponent<CLockableComponent>();

	// We want to supply interaction verbs.
	auto m_interactor = pEntity->GetOrCreateComponent<CEntityInteractionComponent>();
	if (m_interactor)
	{
		m_interactor->AddInteraction(std::make_shared<CInteractionOpen>(this));
		m_interactor->AddInteraction(std::make_shared<CInteractionClose>(this));
	}

	OnResetState();
}


void CAnimatedDoorComponent::ProcessEvent(SEntityEvent& event)
{
	switch (event.event)
	{
		// Physicalize on level start for Launcher
		case ENTITY_EVENT_START_LEVEL:

			// Editor specific, physicalize on reset, property change or transform change
		case ENTITY_EVENT_RESET:
		case ENTITY_EVENT_EDITOR_PROPERTY_CHANGED:
		case ENTITY_EVENT_XFORM_FINISHED_EDITOR:
			OnResetState();
			break;
	}
}


void CAnimatedDoorComponent::SerializeProperties(Serialization::IArchive& archive)
{
	if (archive.isInput())
	{
		OnResetState();
	}
}


void CAnimatedDoorComponent::OnResetState()
{
}
