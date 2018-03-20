#include <StdAfx.h>

#include "Pet.h"

namespace Chrysalis
{
void CPetComponent::Register(Schematyc::CEnvRegistrationScope& componentScope)
{
}


void CPetComponent::ReflectType(Schematyc::CTypeDesc<CPetComponent>& desc)
{
	desc.SetGUID("{03DCFDAE-6F31-41D3-9FF5-26F79BB04278}"_cry_guid);
	desc.SetEditorCategory("Actors");
	desc.SetLabel("Pet");
	desc.SetDescription("No description.");
	desc.SetIcon("icons:ObjectTypes/light.ico");
	desc.SetComponentFlags({ IEntityComponent::EFlags::Singleton });
}


void CPetComponent::Initialize()
{
	const auto pEntity = GetEntity();

	// Get it into a known state.
	OnResetState();
}


void CPetComponent::ProcessEvent(SEntityEvent& event)
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

		case ENTITY_EVENT_UPDATE:
			break;
	}
}


void CPetComponent::OnResetState()
{
}
}