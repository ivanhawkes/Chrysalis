#include <StdAfx.h>

#include "SpellParticipantComponent.h"
#include <CryCore/StaticInstanceList.h>
#include "CrySchematyc/Env/Elements/EnvComponent.h"
#include "CrySchematyc/Env/IEnvRegistrar.h"
#include "Components/Player/PlayerComponent.h"
#include <entt/core/type_traits.hpp>
#include <ECS/ECS.h>
#include <ECS/Components/Spells/Spell.h>


namespace Chrysalis
{
static void RegisterSpellParticipantComponent(Schematyc::IEnvRegistrar& registrar)
{
	Schematyc::CEnvRegistrationScope scope = registrar.Scope(IEntity::GetEntityScopeGUID());
	{
		Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(CSpellParticipantComponent));
		// Functions
		{
		}
	}
}


void CSpellParticipantComponent::ReflectType(Schematyc::CTypeDesc<CSpellParticipantComponent>& desc)
{
	desc.SetGUID(CSpellParticipantComponent::IID());
	desc.SetEditorCategory("Spells");
	desc.SetLabel("Spell Participant");
	desc.SetDescription("Allow interaction with spells for this entity.");
	desc.SetIcon("icons:ObjectTypes/light.ico");

	desc.SetComponentFlags({IEntityComponent::EFlags::Singleton});

	// Keep a collection of spells available for use.
}


// ***
// *** IEntityComponent
// ***


void CSpellParticipantComponent::Initialize()
{
	if (!m_isInit)
	{
		// Get the ECS actor registry.
		auto& actorRegistry = ECS::Simulation.GetActorRegistry();

		// Need a new entity bound to this one for both their lives.
		m_ecsEntity = actorRegistry.create();

		// Name component.
		actorRegistry.emplace<ECS::Name>(m_ecsEntity, m_pEntity->GetName(), m_pEntity->GetName());

		// Health component.
		actorRegistry.emplace<ECS::Health>(m_ecsEntity, ECS::AttributeType {120.0f, 0.0f, 0.0f});

		// Qi component.
		actorRegistry.emplace<ECS::Qi>(m_ecsEntity, ECS::AttributeType {120.0f, 0.0f, 0.0f});

		// Movement factor will allow us to change their movement rate more easily with spells.
		actorRegistry.emplace<ECS::MovementFactor>(m_ecsEntity, ECS::MovementFactor {1.0f});

		m_isInit = true;
	}
}


void CSpellParticipantComponent::ProcessEvent(const SEntityEvent& event)
{
	switch (event.event)
	{
		case EEntityEvent::Remove:
		{
			// Clean up the ECS entity, as it's no longer needed.
			auto& registry = ECS::Simulation.GetActorRegistry();
			registry.destroy(m_ecsEntity);
			break;
		}
	}
}


// ***
// *** CSpellParticipantComponent
// ***

CRY_STATIC_AUTO_REGISTER_FUNCTION(&RegisterSpellParticipantComponent)
}