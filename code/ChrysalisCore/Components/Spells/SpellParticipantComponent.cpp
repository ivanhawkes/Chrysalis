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
	
	// TEST: Seeing how well components can work in the CRYENGINE interface.
	desc.AddMember(&CSpellParticipantComponent::m_health, 'hlth', "Health", "Health", "Health", ECS::Health {});
	desc.AddMember(&CSpellParticipantComponent::m_qi, 'qiqi', "Qi", "Qi", "Qi", ECS::Qi {});
}


// ***
// *** IEntityComponent
// ***


void CSpellParticipantComponent::Initialize()
{
	// Get the ECS actor registry.
	auto actorRegistry = ECS::Simulation.GetActorRegistry();

	// Need a new entity bound to this one for both their lives.
	m_ecsEntity = actorRegistry->create();

	// Name component.
	actorRegistry->emplace<ECS::Name>(m_ecsEntity, m_pEntity->GetName(), m_pEntity->GetName());

	// Health component.
	ECS::AttributeType<float> health {120.0f, 0.0f, 0.0f};
	m_health = actorRegistry->emplace<ECS::Health>(m_ecsEntity, health);

	// Qi component.
	ECS::AttributeType<float> qi {100.0f, 0.0f, 0.0f};
	m_qi = actorRegistry->emplace<ECS::Qi>(m_ecsEntity, qi);

	// TEST: adding a tag...
	actorRegistry->emplace<ECS::CrowdControlNone>(m_ecsEntity);
	actorRegistry->emplace<ECS::SaltComponent>(m_ecsEntity);
	actorRegistry->emplace<ECS::PepperComponent>(m_ecsEntity);
}


void CSpellParticipantComponent::ProcessEvent(const SEntityEvent& event)
{
	switch (event.event)
	{
		case EEntityEvent::Remove:
		{
			// Clean up the ECS entity, as it's no longer needed.
			auto registry = ECS::Simulation.GetActorRegistry();
			registry->destroy(m_ecsEntity);
			break;
		}
	}
}


// ***
// *** CSpellParticipantComponent
// ***

CRY_STATIC_AUTO_REGISTER_FUNCTION(&RegisterSpellParticipantComponent)
}