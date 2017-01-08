#include <StdAfx.h>

#include "AnimatedDoorComponent.h"
#include "Plugin/ChrysalisCorePlugin.h"
#include <Entities/Lockable/LockableComponent.h>
#include <CrySerialization/Decorators/Resources.h>


CRYREGISTER_CLASS(CAnimatedDoorComponent)

class CDoorRegistrator : public IEntityRegistrator
{
	void Register() override
	{
		RegisterEntityWithDefaultComponent<CAnimatedDoorComponent>("AnimatedDoor", "Doors", "Light.bmp");
	}

	void Unregister() override {};
};

CDoorRegistrator g_doorRegistrator;


void CAnimatedDoorComponent::Initialize()
{
	auto pEntity = GetEntity();
	
	// TODO: The geometry should come from a Geom component instead.
	// Add one here.

	auto m_lockableExtension = pEntity->GetOrCreateComponent<CLockableComponent>();

	// We want to supply interaction verbs.
	auto m_interactor = pEntity->GetOrCreateComponent<CEntityInteractionComponent>();
	if (m_interactor)
	{
		auto openInteractPtr = std::make_shared<CInteractionOpen>(this);
		m_interactor->AddInteraction(openInteractPtr);

		auto closeInteractPtr = std::make_shared<CInteractionClose>(this);
		m_interactor->AddInteraction(closeInteractPtr);
	}

	Reset();
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
			Reset();
			break;
	}
}


void CAnimatedDoorComponent::SerializeProperties(Serialization::IArchive& archive)
{
	archive(Serialization::ModelFilename(m_geometry), "Geometry", "Geometry");
	archive(m_mass, "Mass", "Mass");

	if (!archive.isInput())
	{
		Reset();
	}
}


void CAnimatedDoorComponent::Reset()
{
	if (strlen(m_geometry) > 0)
	{
		GetEntity()->LoadGeometry(0, m_geometry);

		SEntityPhysicalizeParams params;
		params.type = PE_RIGID;
		params.mass = m_mass;

		GetEntity()->Physicalize(params);
	}
}
