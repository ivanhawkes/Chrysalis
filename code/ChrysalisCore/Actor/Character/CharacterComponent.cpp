#include <StdAfx.h>

#include "CharacterComponent.h"
#include <Actor/ActorComponent.h>
#include <Actor/Movement/StateMachine/ActorStateEvents.h>
#include <Actor/Movement/StateMachine/ActorStateUtility.h>
#include <Actor/Character/CharacterAttributesComponent.h>


namespace Chrysalis
{
void CCharacterComponent::Register(Schematyc::CEnvRegistrationScope& componentScope)
{
}


void CCharacterComponent::ReflectType(Schematyc::CTypeDesc<CCharacterComponent>& desc)
{
	desc.SetGUID(CCharacterComponent::IID());
	desc.SetEditorCategory("Actors");
	desc.SetLabel("Character");
	desc.SetDescription("No description.");
	desc.SetIcon("icons:ObjectTypes/light.ico");
	desc.SetComponentFlags({ IEntityComponent::EFlags::Transform });
}


void CCharacterComponent::Initialize()
{
	const auto pEntity = GetEntity();

	// All characters are actors.
	m_pActorComponent = pEntity->GetOrCreateComponent<CActorComponent>();

	// Manage attributes.
	m_pCharacterAttributesComponent = pEntity->GetOrCreateComponent<CCharacterAttributesComponent>();

	// Get it into a known state.
	OnResetState();
}


void CCharacterComponent::ProcessEvent(SEntityEvent& event)
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


void CCharacterComponent::OnResetState()
{
}
}