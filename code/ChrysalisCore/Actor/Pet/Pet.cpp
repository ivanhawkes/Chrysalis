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
	// Get it into a known state.
	OnResetState();
}


void CPetComponent::ProcessEvent(const SEntityEvent& event)
{
	switch (event.event)
	{
		// Physicalize on level start for Launcher
		case EEntityEvent::LevelStarted:

			// Editor specific, physicalize on reset, property change or transform change
		case EEntityEvent::Reset:
		case EEntityEvent::EditorPropertyChanged:
		case EEntityEvent::TransformChangeFinishedInEditor:
			OnResetState();
			break;

		case EEntityEvent::Update:
			break;
	}
}


void CPetComponent::OnResetState()
{
}
}